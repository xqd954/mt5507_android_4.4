package com.mediatek.mmpcm;

public class MetaData {
    private String title;
    private String director;
    private String copyright;
    private String year;
    private String genre;
    private String artist;
    private String album;
    private int duration;
    private int bitrate;

    public void setMetaData(String mtitle, String mdirector, String mcopyright,
            String myear, String mgenre, String martist, String malbum,
            int mduration, int mbitrate) {
        this.title = mtitle;
        this.director = mdirector;
        this.copyright = mcopyright;
        this.year = myear;
        this.genre = mgenre;
        this.artist = martist;
        this.album = malbum;
        this.duration = mduration;
        this.bitrate = mbitrate;
    }
    
    public String getTitle(){
        return title;
    }
    
    public String getDirector(){
        return director;
    }
    
    public String getCopyright(){
        return copyright;
    }
    
    public String getYear(){
        return year;
    }
    
    public String getGenre(){
        return genre;
    }
    
    public String getArtist(){
        return artist;
    }
    
    public String getAlbum(){
        return album;
    }
    
    public int getDuration(){
        return duration;
    }
    
    public int getBitrate(){
        return bitrate;
    }
}
