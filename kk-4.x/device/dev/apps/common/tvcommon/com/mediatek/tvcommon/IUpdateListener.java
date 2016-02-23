package com.mediatek.tvcommon;

import android.os.Binder;
import android.os.IBinder;
import android.os.IInterface;
import android.os.Parcel;
import android.os.RemoteException;

public interface IUpdateListener extends IInterface {
	public static abstract class Stub extends Binder implements IUpdateListener {
		private static final String DESCRIPTOR = "IUpdateListener";

		public Stub() {
			this.attachInterface(this, DESCRIPTOR);
		}

		public static IUpdateListener asInterface(IBinder obj) {
			if ((obj == null)) {
				return null;
			}
			android.os.IInterface iin = (IInterface) obj
					.queryLocalInterface(DESCRIPTOR);
			if (((iin != null) && (iin instanceof IUpdateListener))) {
				return ((IUpdateListener) iin);
			}
			return new IUpdateListener.Stub.Proxy(obj);
		}

		public IBinder asBinder() {
			return this;
		}

		@Override
		public boolean onTransact(int code, Parcel data, Parcel reply, int flags)
				throws RemoteException {
			switch (code) {
			case INTERFACE_TRANSACTION: {
				reply.writeString(DESCRIPTOR);
				return true;
			}
			case TRANSACTION_onUpdate: {
				data.enforceInterface(DESCRIPTOR);
				int reason = data.readInt();

				switch (reason) {
				case UpdateReason.RESET_CH_NUM: {
					int inputID = data.readInt();
					int resetNum = data.readInt();
					int newNum = data.readInt();
					this.onUpdate(reason, "", inputID, resetNum, newNum);
					reply.writeNoException();
					break;
				}

				case UpdateReason.RESET_CH_NAME: {
					String name = data.readString();
					int inputID = data.readInt();
					int num = data.readInt();
					this.onUpdate(reason, name, inputID, num);
					reply.writeNoException();
					break;
				}

				case UpdateReason.RESET_CH_MASK: {
					int inputID = data.readInt();
					int num = data.readInt();
					int mask = data.readInt();
					this.onUpdate(reason, "", inputID, num, mask);
					reply.writeNoException();
					break;
				}

				case UpdateReason.RESET_CH_FREQ: {
					int inputID = data.readInt();
					int num = data.readInt();
					int freq = data.readInt();
					this.onUpdate(reason, "", inputID, num, freq);
					reply.writeNoException();
					break;
				}

				case UpdateReason.DELETE_CH: {
					int inputID = data.readInt();
					int delNum = data.readInt();
					this.onUpdate(reason, "", inputID, delNum);
					reply.writeNoException();
					break;
				}

				case UpdateReason.INSERT_CH: {
					int inputID = data.readInt();
					int dstNum = data.readInt();
					int srcNum = data.readInt();
					this.onUpdate(reason, "", inputID, dstNum, srcNum);
					reply.writeNoException();
					break;
				}

				case UpdateReason.SWAP_CH: {
					int inputID = data.readInt();
					int lNum = data.readInt();
					int rNum = data.readInt();
					this.onUpdate(reason, "", inputID, lNum, rNum);
					reply.writeNoException();
					break;
				}

				case UpdateReason.SET_TUNNER:
				case UpdateReason.SCAN_COMPLETE:
				case UpdateReason.ICH_NOTIFY:
				case UpdateReason.CLEAR_CHANNELS: {
					this.onUpdate(reason, "");
					break;
				}

				default:
					break;
				}

				return true;
			}
			}
			return super.onTransact(code, data, reply, flags);
		}

		private static class Proxy implements IUpdateListener {
			private IBinder mRemote;

			Proxy(IBinder remote) {
				mRemote = remote;
			}

			public IBinder asBinder() {
				return mRemote;
			}

			public void onUpdate(int reason, String str, int... para)
					throws RemoteException {
				Parcel data = Parcel.obtain();
				Parcel reply = Parcel.obtain();

				data.writeInterfaceToken(DESCRIPTOR);
				data.writeInt(reason);

				switch (reason) {
				case UpdateReason.RESET_CH_NUM: {
					int inputID = para[0];
					data.writeInt(inputID);
					int num = para[1];
					data.writeInt(num);
					int newNum = para[2];
					data.writeInt(newNum);
					break;
				}

				case UpdateReason.RESET_CH_NAME: {
					String name = str;
					data.writeString(name);
					int inputID = para[0];
					data.writeInt(inputID);
					int num = para[1];
					data.writeInt(num);
					break;
				}

				case UpdateReason.RESET_CH_MASK: {
					int inputID = para[0];
					data.writeInt(inputID);
					int num = para[1];
					data.writeInt(num);
					int mask = para[2];
					data.writeInt(mask);
					break;
				}

				case UpdateReason.RESET_CH_FREQ: {
					int inputID = para[0];
					data.writeInt(inputID);
					int num = para[1];
					data.writeInt(num);
					int freq = para[2];
					data.writeInt(freq);
					break;
				}

				case UpdateReason.DELETE_CH: {
					int inputID = para[0];
					data.writeInt(inputID);
					int num = para[1];
					data.writeInt(num);
					break;
				}

				case UpdateReason.INSERT_CH: {
					int inputID = para[0];
					data.writeInt(inputID);
					int dstNum = para[1];
					data.writeInt(dstNum);
					int srcNum = para[2];
					data.writeInt(srcNum);
					break;
				}

				case UpdateReason.SWAP_CH: {
					int inputID = para[0];
					data.writeInt(inputID);
					int lNum = para[1];
					data.writeInt(lNum);
					int rNum = para[2];
					data.writeInt(rNum);
					break;
				}

				case UpdateReason.SET_TUNNER:
				case UpdateReason.SCAN_COMPLETE:
				case UpdateReason.ICH_NOTIFY:
				case UpdateReason.CLEAR_CHANNELS: {
					break;
				}

				default:
					break;
				}

				try {
					// TODO
					mRemote.transact(Stub.TRANSACTION_onUpdate, data, reply, 0);
					reply.readException();
				} finally {
					reply.recycle();
					data.recycle();
				}
			}
		}

		static final int TRANSACTION_onUpdate = (IBinder.FIRST_CALL_TRANSACTION + 0);
	}

	public void onUpdate(int reason, String str, int... para)
			throws RemoteException;
}
