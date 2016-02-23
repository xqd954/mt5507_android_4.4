/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.os;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.security.GeneralSecurityException;
import java.security.PublicKey;
import java.security.Signature;
import java.security.SignatureException;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.Collection;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import org.apache.harmony.security.asn1.BerInputStream;
import org.apache.harmony.security.pkcs7.ContentInfo;
import org.apache.harmony.security.pkcs7.SignedData;
import org.apache.harmony.security.pkcs7.SignerInfo;
import org.apache.harmony.security.provider.cert.X509CertImpl;

// Begin added by TCL pengpeng, mail: pengpeng01@tcl.com
import android.content.SharedPreferences;
// End of TCL

/**
 * RecoverySystem contains methods for interacting with the Android
 * recovery system (the separate partition that can be used to install
 * system updates, wipe user data, etc.)
 */
public class RecoverySystem {
    private static final String TAG = "RecoverySystem";

    /**
     * Default location of zip file containing public keys (X509
     * certs) authorized to sign OTA updates.
     */
    private static final File DEFAULT_KEYSTORE =
        new File("/system/etc/security/otacerts.zip");

    /** Send progress to listeners no more often than this (in ms). */
    private static final long PUBLISH_PROGRESS_INTERVAL_MS = 500;

    /** Used to communicate with recovery.  See bootable/recovery/recovery.c. */
    private static File RECOVERY_DIR = new File("/cache/recovery");
    private static File COMMAND_FILE = new File(RECOVERY_DIR, "command");
    private static File LOG_FILE = new File(RECOVERY_DIR, "log");
    private static String LAST_PREFIX = "last_";

    // Length limits for reading files.
    private static int LOG_FILE_MAX_LENGTH = 64 * 1024;

    /**
     * Interface definition for a callback to be invoked regularly as
     * verification proceeds.
     */
    public interface ProgressListener {
        /**
         * Called periodically as the verification progresses.
         *
         * @param progress  the approximate percentage of the
         *        verification that has been completed, ranging from 0
         *        to 100 (inclusive).
         */
        public void onProgress(int progress);
    }

    /** @return the set of certs that can be used to sign an OTA package. */
    private static HashSet<Certificate> getTrustedCerts(File keystore)
        throws IOException, GeneralSecurityException {
        HashSet<Certificate> trusted = new HashSet<Certificate>();
        if (keystore == null) {
            keystore = DEFAULT_KEYSTORE;
        }
        ZipFile zip = new ZipFile(keystore);
        try {
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            Enumeration<? extends ZipEntry> entries = zip.entries();
            while (entries.hasMoreElements()) {
                ZipEntry entry = entries.nextElement();
                InputStream is = zip.getInputStream(entry);
                try {
                    trusted.add(cf.generateCertificate(is));
                } finally {
                    is.close();
                }
            }
        } finally {
            zip.close();
        }
        return trusted;
    }

    /**
     * Verify the cryptographic signature of a system update package
     * before installing it.  Note that the package is also verified
     * separately by the installer once the device is rebooted into
     * the recovery system.  This function will return only if the
     * package was successfully verified; otherwise it will throw an
     * exception.
     *
     * Verification of a package can take significant time, so this
     * function should not be called from a UI thread.  Interrupting
     * the thread while this function is in progress will result in a
     * SecurityException being thrown (and the thread's interrupt flag
     * will be cleared).
     *
     * @param packageFile  the package to be verified
     * @param listener     an object to receive periodic progress
     * updates as verification proceeds.  May be null.
     * @param deviceCertsZipFile  the zip file of certificates whose
     * public keys we will accept.  Verification succeeds if the
     * package is signed by the private key corresponding to any
     * public key in this file.  May be null to use the system default
     * file (currently "/system/etc/security/otacerts.zip").
     *
     * @throws IOException if there were any errors reading the
     * package or certs files.
     * @throws GeneralSecurityException if verification failed
     */
    public static void verifyPackage(File packageFile,
                                     ProgressListener listener,
                                     File deviceCertsZipFile)
        throws IOException, GeneralSecurityException {
        long fileLen = packageFile.length();

        RandomAccessFile raf = new RandomAccessFile(packageFile, "r");
        try {
            int lastPercent = 0;
            long lastPublishTime = System.currentTimeMillis();
            if (listener != null) {
                listener.onProgress(lastPercent);
            }

            raf.seek(fileLen - 6);
            byte[] footer = new byte[6];
            raf.readFully(footer);

            if (footer[2] != (byte)0xff || footer[3] != (byte)0xff) {
                throw new SignatureException("no signature in file (no footer)");
            }

            int commentSize = (footer[4] & 0xff) | ((footer[5] & 0xff) << 8);
            int signatureStart = (footer[0] & 0xff) | ((footer[1] & 0xff) << 8);

            byte[] eocd = new byte[commentSize + 22];
            raf.seek(fileLen - (commentSize + 22));
            raf.readFully(eocd);

            // Check that we have found the start of the
            // end-of-central-directory record.
            if (eocd[0] != (byte)0x50 || eocd[1] != (byte)0x4b ||
                eocd[2] != (byte)0x05 || eocd[3] != (byte)0x06) {
                throw new SignatureException("no signature in file (bad footer)");
            }

            for (int i = 4; i < eocd.length-3; ++i) {
                if (eocd[i  ] == (byte)0x50 && eocd[i+1] == (byte)0x4b &&
                    eocd[i+2] == (byte)0x05 && eocd[i+3] == (byte)0x06) {
                    throw new SignatureException("EOCD marker found after start of EOCD");
                }
            }

            // The following code is largely copied from
            // JarUtils.verifySignature().  We could just *call* that
            // method here if that function didn't read the entire
            // input (ie, the whole OTA package) into memory just to
            // compute its message digest.

            BerInputStream bis = new BerInputStream(
                new ByteArrayInputStream(eocd, commentSize+22-signatureStart, signatureStart));
            ContentInfo info = (ContentInfo)ContentInfo.ASN1.decode(bis);
            SignedData signedData = info.getSignedData();
            if (signedData == null) {
                throw new IOException("signedData is null");
            }
            Collection encCerts = signedData.getCertificates();
            if (encCerts.isEmpty()) {
                throw new IOException("encCerts is empty");
            }
            // Take the first certificate from the signature (packages
            // should contain only one).
            Iterator it = encCerts.iterator();
            X509Certificate cert = null;
            if (it.hasNext()) {
                cert = new X509CertImpl((org.apache.harmony.security.x509.Certificate)it.next());
            } else {
                throw new SignatureException("signature contains no certificates");
            }

            List sigInfos = signedData.getSignerInfos();
            SignerInfo sigInfo;
            if (!sigInfos.isEmpty()) {
                sigInfo = (SignerInfo)sigInfos.get(0);
            } else {
                throw new IOException("no signer infos!");
            }

            // Check that the public key of the certificate contained
            // in the package equals one of our trusted public keys.

            HashSet<Certificate> trusted = getTrustedCerts(
                deviceCertsZipFile == null ? DEFAULT_KEYSTORE : deviceCertsZipFile);

            PublicKey signatureKey = cert.getPublicKey();
            boolean verified = false;
            for (Certificate c : trusted) {
                if (c.getPublicKey().equals(signatureKey)) {
                    verified = true;
                    break;
                }
            }
            if (!verified) {
                throw new SignatureException("signature doesn't match any trusted key");
            }

            // The signature cert matches a trusted key.  Now verify that
            // the digest in the cert matches the actual file data.

            // The verifier in recovery only handles SHA1withRSA and
            // SHA256withRSA signatures.  SignApk chooses which to use
            // based on the signature algorithm of the cert:
            //
            //    "SHA256withRSA" cert -> "SHA256withRSA" signature
            //    "SHA1withRSA" cert   -> "SHA1withRSA" signature
            //    "MD5withRSA" cert    -> "SHA1withRSA" signature (for backwards compatibility)
            //    any other cert       -> SignApk fails
            //
            // Here we ignore whatever the cert says, and instead use
            // whatever algorithm is used by the signature.

            String da = sigInfo.getDigestAlgorithm();
            String dea = sigInfo.getDigestEncryptionAlgorithm();
            String alg = null;
            if (da == null || dea == null) {
                // fall back to the cert algorithm if the sig one
                // doesn't look right.
                alg = cert.getSigAlgName();
            } else {
                alg = da + "with" + dea;
            }
            Signature sig = Signature.getInstance(alg);
            sig.initVerify(cert);

            // The signature covers all of the OTA package except the
            // archive comment and its 2-byte length.
            long toRead = fileLen - commentSize - 2;
            long soFar = 0;
            raf.seek(0);
            byte[] buffer = new byte[4096];
            boolean interrupted = false;
            while (soFar < toRead) {
                interrupted = Thread.interrupted();
                if (interrupted) break;
                int size = buffer.length;
                if (soFar + size > toRead) {
                    size = (int)(toRead - soFar);
                }
                int read = raf.read(buffer, 0, size);
                sig.update(buffer, 0, read);
                soFar += read;

                if (listener != null) {
                    long now = System.currentTimeMillis();
                    int p = (int)(soFar * 100 / toRead);
                    if (p > lastPercent &&
                        now - lastPublishTime > PUBLISH_PROGRESS_INTERVAL_MS) {
                        lastPercent = p;
                        lastPublishTime = now;
                        listener.onProgress(lastPercent);
                    }
                }
            }
            if (listener != null) {
                listener.onProgress(100);
            }

            if (interrupted) {
                throw new SignatureException("verification was interrupted");
            }

            if (!sig.verify(sigInfo.getEncryptedDigest())) {
                throw new SignatureException("signature digest verification failed");
            }
        } finally {
            raf.close();
        }
    }

    // Begin added by TCL pengpeng, mail: pengpeng01@tcl.com
    private static String updatePrefName = "system_update_pref";
    private static String IS_NET_UPDATE = "isNetUpdate";

    /**
     * Get update type by pref.
     *
     * @param context the Context to use
     *
     * @return net update?
     *
     */
    private static boolean isNetUpdate(Context context) {
        SharedPreferences sp = context.getSharedPreferences(updatePrefName,
                                       Context.MODE_PRIVATE);
        Boolean netUpdate = sp.getBoolean(IS_NET_UPDATE, false);

        // set default state
        SharedPreferences.Editor editor = sp.edit();
        editor.putBoolean(IS_NET_UPDATE, false);
        // commit
        editor.commit();

        return netUpdate;
    }

    private static String nextRebootPrefName = "nextRebootUpdate";
    private static String IS_NEXT_REBOOT = "isNextRebootUpdate";
    /**
     * Get nextRebootUpdate type by pref.
     *
     * @param context the Context to use
     *
     * @return net update?
     *
     */
    private static boolean isNextRebootUpdate(Context context) {
        SharedPreferences sp = context.getSharedPreferences(nextRebootPrefName,
                                       Context.MODE_PRIVATE);
        Boolean nextReboot = sp.getBoolean(IS_NEXT_REBOOT, false);

        // set default state
        SharedPreferences.Editor editor = sp.edit();
        editor.putBoolean(IS_NEXT_REBOOT, false);
        // commit
        editor.commit();

        return nextReboot;
    }
//factory reset backup
    private static String backupPrefName = "checkRecoverFactorySettingPre";
    private static String IS_BACKUP = "needClear";
    /**
     * Get nextRebootUpdate type by pref.
     *
     * @param context the Context to use
     *
     * @return net update?
     *
     */
    private static boolean isFactoryResetBackup(Context context) {
        SharedPreferences sp = context.getSharedPreferences(backupPrefName,
                                       Context.MODE_PRIVATE);
        Boolean backup = sp.getBoolean(IS_BACKUP, false);
        Log.i(TAG, "checkRecoverFactorySettingPre needClear: " + backup);
        // set default state
        SharedPreferences.Editor editor = sp.edit();
        editor.putBoolean(IS_BACKUP, false);
        // commit
        editor.commit();

        return backup;
    }
//end
    // End of TCL

    /**
     * Reboots the device in order to install the given update
     * package.
     * Requires the {@link android.Manifest.permission#REBOOT} permission.
     *
     * @param context      the Context to use
     * @param packageFile  the update package to install.  Must be on
     * a partition mountable by recovery.  (The set of partitions
     * known to recovery may vary from device to device.  Generally,
     * /cache and /data are safe.)
     *
     * @throws IOException  if writing the recovery command file
     * fails, or if the reboot itself fails.
     */
    public static void installPackage(Context context, File packageFile)
        throws IOException {
        String filename = packageFile.getCanonicalPath();
        // Begin added by TCL pengpeng, mail: pengpeng01@tcl.com
        if (filename.startsWith("/storage/sdcard1")) {
            String basename = filename.substring(filename.lastIndexOf("/")+1);
            filename = "/mnt/sdcard/" + basename;
        }
        // End of TCL
        Log.w(TAG, "!!! REBOOTING TO INSTALL " + filename + " !!!");
        String arg = "--update_package=" + filename +
            "\n--locale=" + Locale.getDefault().toString();

        // Begin added by TCL pengpeng, mail: pengpeng01@tcl.com
        if (isNetUpdate(context)) {
            arg = arg + "\n--remove_package";
            Log.d(TAG, "arg: " + arg);
        }

        if (isNextRebootUpdate(context)) {
            Log.d(TAG, "is next reboot \n");
            writeCommand(context, arg);
        }
        else{
        // End of TCL
            bootCommand(context, arg);
	}
    }

    /**
     * Reboots the device and wipes the user data partition.  This is
     * sometimes called a "factory reset", which is something of a
     * misnomer because the system partition is not restored to its
     * factory state.
     * Requires the {@link android.Manifest.permission#REBOOT} permission.
     *
     * @param context  the Context to use
     *
     * @throws IOException  if writing the recovery command file
     * fails, or if the reboot itself fails.
     */
    public static void rebootWipeUserData(Context context) throws IOException {
        final ConditionVariable condition = new ConditionVariable();

        Intent intent = new Intent("android.intent.action.MASTER_CLEAR_NOTIFICATION");
        context.sendOrderedBroadcastAsUser(intent, UserHandle.OWNER,
                android.Manifest.permission.MASTER_CLEAR,
                new BroadcastReceiver() {
                    @Override
                    public void onReceive(Context context, Intent intent) {
                        condition.open();
                    }
                }, null, 0, null, null);

        // Block until the ordered broadcast has completed.
        condition.block();
        //factory reset backup
        if (isFactoryResetBackup(context)) {
                bootCommand(context, "--wipe_data\n--backup_file\n--locale=" + Locale.getDefault().toString());
        }
        else
        //end 
    	    bootCommand(context, "--wipe_data\n--locale=" + Locale.getDefault().toString());
    }

    /**
     * Reboot into the recovery system to wipe the /cache partition.
     * @throws IOException if something goes wrong.
     */
    public static void rebootWipeCache(Context context) throws IOException {
        bootCommand(context, "--wipe_cache\n--locale=" + Locale.getDefault().toString());
    }

    // Begin added by TCL pengpeng, mail: pengpeng01@tcl.com
    /**
     * Reboot into the recovery system to repair system.
     * @throws IOException if something goes wrong.
     */
    public static void rebootRepairSystem(Context context) throws IOException {
        bootCommand(context, "--repair_system\n--locale=" + Locale.getDefault().toString());
    }
    // End of TCL

    /**
     * Reboot into the recovery system with the supplied argument.
     * @param arg to pass to the recovery utility.
     * @throws IOException if something goes wrong.
     */
    private static void bootCommand(Context context, String arg) throws IOException {
        RECOVERY_DIR.mkdirs();  // In case we need it
        COMMAND_FILE.delete();  // In case it's not writable
        LOG_FILE.delete();

        FileWriter command = new FileWriter(COMMAND_FILE);
        try {
            command.write(arg);
            command.write("\n");
        } finally {
            command.close();
        }

        // Having written the command file, go ahead and reboot
        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        pm.reboot("recovery");

        throw new IOException("Reboot failed (no permissions?)");
    }

    //TCL Android Patch Begin
    /**
     * Write the supplied argument but not reboot.
     * @param arg to pass to the recovery utility.
     * @throws IOException if something goes wrong.
     */
    private static void writeCommand(Context context, String arg) throws IOException {
        RECOVERY_DIR.mkdirs();  // In case we need it
        COMMAND_FILE.delete();  // In case it's not writable
        LOG_FILE.delete();

        FileWriter command = new FileWriter(COMMAND_FILE);
        try {
            command.write(arg);
            command.write("\n");
        } finally {
            command.close();
        }
        FileWriter misc_data = new FileWriter("dev/block/mmcblk0p12");
        char [] misc_command = new char [32];
        String data1 = "boot-recovery";
        for (int i=0; i<data1.length(); i++) misc_command[i] = data1.charAt(i);
        char [] misc_status = new char [32];
        char [] misc_recovery = new char [1024];
        String data2 = "recovery\n" + arg;
	for (int j=0; j<data2.length(); j++) misc_recovery[j] = data2.charAt(j);
        try {
            misc_data.write(misc_command);
            misc_data.write(misc_status);
            misc_data.write(misc_recovery);
        } finally {
            misc_data.close();
        }

    }
    //TCL end

    /**
     * Called after booting to process and remove recovery-related files.
     * @return the log file from recovery, or null if none was found.
     *
     * @hide
     */
    public static String handleAftermath() {
        // Record the tail of the LOG_FILE
        String log = null;
        try {
            log = FileUtils.readTextFile(LOG_FILE, -LOG_FILE_MAX_LENGTH, "...\n");
        } catch (FileNotFoundException e) {
            Log.i(TAG, "No recovery log file");
        } catch (IOException e) {
            Log.e(TAG, "Error reading recovery log", e);
        }

        // Delete everything in RECOVERY_DIR except those beginning
        // with LAST_PREFIX
        String[] names = RECOVERY_DIR.list();
        for (int i = 0; names != null && i < names.length; i++) {
            if (names[i].startsWith(LAST_PREFIX)) continue;
            File f = new File(RECOVERY_DIR, names[i]);
            if (!f.delete()) {
                Log.e(TAG, "Can't delete: " + f);
            } else {
                Log.i(TAG, "Deleted: " + f);
            }
        }

        return log;
    }

    private void RecoverySystem() { }  // Do not instantiate
}
