package android.media;

import android.util.Log;

/**
 * SubtitleTrackInfo Class can be used to get subtitle track info 
 * 
 * 
 */
public class SubtitleTrackInfo
{
    private int mSubtitleType;
    private int mSubtitleCodeType;
    private String mszSubtitleLanguage;
    private int mSubtitleLanguageType;
    
    private String mszSubtitleTitle;
    /**
     * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
     */
    public SubtitleTrackInfo(int SubtitleType,
                         int SubtitleCodeType,
                         String szSubtitleLanguage,
                         int SubtitleLanguageType)
    {
        super();
        this.mSubtitleType = SubtitleType;
        this.mSubtitleCodeType = SubtitleCodeType;
        this.mszSubtitleLanguage = szSubtitleLanguage;
        this.mSubtitleLanguageType = SubtitleLanguageType;
    }
    
    /**
     * This Constructor used for MtkMediaPlayer classes. Please avoid to use this class directly.
     */
    public SubtitleTrackInfo(int SubtitleType, 
                         int SubtitleCodeType,
                         String szSubtitleLanguage,
                         String szSubtitleTitle)
    {
        super();
        this.mSubtitleType = SubtitleType;
        this.mSubtitleCodeType = SubtitleCodeType;
        this.mszSubtitleLanguage = szSubtitleLanguage;
        this.mszSubtitleTitle = szSubtitleTitle;
    }
    /**
     *   This API will get subtitle mSubtitleType
     *
     *   @return mSubtitleType 
     */
    public int getSubtitleType()
    {
        return mSubtitleType;
    }
    
    /**
     *   This API will get subtitle mSubtitleCodeType
     *
     *   @return mSubtitleCodeType 
     */
    public int getSubtitleCodeType()
    {
        return mSubtitleCodeType;
    }
    
    /**
     *   This API will get subtitle mszSubtitleLanguage
     *
     *   @return mszSubtitleLanguage 
     */
    public String getSubtitleLanguage()
    {
        return mszSubtitleLanguage;
    }
    
    /**
     *   This API will get subtitle mSubtitleLanguageType
     *
     *   @return mSubtitleLanguageType 
     */
    public int getSubtitleLanguageType()
    {
        return mSubtitleLanguageType;
    }
    
    /**
     *   This API will get subtitle mszSubtitleTitle
     *
     *   @return mszSubtitleTitle 
     */
    public String getSubtitleTitle()
    {
        return mszSubtitleTitle;
    }
}
