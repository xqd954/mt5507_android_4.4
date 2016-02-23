package android.media;

import android.util.Log;

/**
 * AudioTrackInfo Class can be used to get audio track info 
 * 
 * 
 */

public class AudioTrackInfo
{
    private int mAudioTrackNum;
    private int bitRate;
    private int totalPlayTime;
    private int currentPlayTime;
    private int sampleRate;
    private int codecID;
    private String year;
    private String title;
    private String album;
    private String artist;

    private int mChannelNumber;
    private String mGenre;
    private String mDirector;
    private String mCopyright;

    private String mLanguage;
    
    /**
     * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
     */
    public AudioTrackInfo(int audioTrackNum, int bitRate, int totalPlayTime, int currentPlayTime,
        int sampleRate, int codecID, String year, String title, String album, String artist,
        int channelNum, String genre, String director, String copyRight)
    {
        super();
        this.mAudioTrackNum = audioTrackNum;
        this.bitRate = bitRate;
        this.totalPlayTime = totalPlayTime;
        this.currentPlayTime = currentPlayTime;
        this.sampleRate = sampleRate;
        this.codecID = codecID;
        this.year = year;
        this.title = title;
        this.album = album;
        this.artist = artist;
        this.mChannelNumber = channelNum;
        this.mGenre = genre;
        this.mDirector = director;
        this.mCopyright = copyRight;
    }
    
    /**
     * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
     */
    public AudioTrackInfo(int audioTrackNum, int bitRate, int totalPlayTime, int currentPlayTime,
        int sampleRate, int codecID, String year, String title, String album, String artist,
        int channelNum, String genre, String director, String copyRight,String language)
    {
        super();
        this.mAudioTrackNum = audioTrackNum;
        this.bitRate = bitRate;
        this.totalPlayTime = totalPlayTime;
        this.currentPlayTime = currentPlayTime;
        this.sampleRate = sampleRate;
        this.codecID = codecID;
        this.year = year;
        this.title = title;
        this.album = album;
        this.artist = artist;
        this.mChannelNumber = channelNum;
        this.mGenre = genre;
        this.mDirector = director;
        this.mCopyright = copyRight;
        this.mLanguage = language;
    }
    
    /**
     *   This API will get audio track number
     *
     *   @return mAudioTrackNum 
     */
    public int getAudioTrackNum()
    {
        return mAudioTrackNum;
    }
    
    /**
     *   This API will get audio track codec ID
     *
     *   @return codecID 
     */
    public int getCodecID()
    {
        return codecID;
    }
    
    /**
     *   This API will get audio track totalPlayTime
     *
     *   @return totalPlayTime 
     */
    public int getTotalPlayTime()
    {
        return totalPlayTime;
    }
    
    /**
     *   This API will get audio track currentPlayTime
     *
     *   @return currentPlayTime 
     */
    public int getCurrentPlayTime()
    {
        return currentPlayTime;
    }
    
    /**
     *   This API will get audio track bitRate
     *
     *   @return bitRate 
     */
    public int getBiteRate()
    {
        return bitRate;
    }
    
    /**
     *   This API will get audio track year
     *
     *   @return year 
     */
    public String getYear()
    {
        return year;
    }
    
    /**
     *   This API will get audio track sampleRate
     *
     *   @return sampleRate 
     */
    public int getSampleRate()
    {
        return sampleRate;
    }
    
    /**
     *   This API will get audio track title
     *
     *   @return title 
     */
    public String getTitle()
    {
         return title;
    }
    
    /**
     *   This API will get audio track album
     *
     *   @return album 
     */
    public String getAlbum()
    {
        return album;
    }
    
    /**
     *   This API will get audio track artist
     *
     *   @return artist 
     */
    public String getArtist()
    {
        return artist;
    }
    
    /**
     *   This API will get audio track mChannelNumber
     *
     *   @return mChannelNumber 
     */
    public int getChannelNumber()
    {
        return mChannelNumber;
    }
    
    /**
     *   This API will get audio track mGenre
     *
     *   @return mGenre 
     */
    public String getGenre()
    {
        return mGenre;
    }
    
    /**
     *   This API will get audio track mDirector
     *
     *   @return mDirector 
     */
    public String getDirector()
    {
        return mDirector;
    }
    
    /**
     *   This API will get audio track mCopyright
     *
     *   @return mCopyright 
     */
    public String getCopyright()
    {
        return mCopyright;
    }
    
    /**
     *   This API will get audio track Language
     *
     *   @return Language ISO code
     */
    public String getLanguage()
    {
        return mLanguage;
    }
}
