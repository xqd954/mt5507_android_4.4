/*
 * jdapistd.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains application interface code for the decompression half
 * of the JPEG library.  These are the "standard" API routines that are
 * used in the normal full-decompression case.  They are not used by a
 * transcoding-only application.  Note that if an application links in
 * jpeg_start_decompress, it will end up linking in the entire decompressor.
 * We thus must separate this file from jdapimin.c to avoid linking the
 * whole decompression library into a transcoder.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"
#include "mtmplayer.h"
#include "mtimage.h"

#ifdef MTM_HWDEC
#include "jmemsys.h"		/* import the system-dependent declarations */
#include <pthread.h>

pthread_mutex_t JpgLibMutex ;
pthread_cond_t JpgStopCond ;
pthread_mutex_t JpgStopMutex ;

static boolean fgJpgMutexInit = FALSE;
#endif

#define JPEG_DEBUG_OFF
#ifdef JPEG_DEBUG_OFF
#define printf
#endif

//#define jdapistd_printf printf
#define jdapistd_printf 

/* Forward declarations */
LOCAL(boolean) output_pass_setup JPP((j_decompress_ptr cinfo));


/*
 * Decompression initialization.
 * jpeg_read_header must be completed before calling this.
 *
 * If a multipass operating mode was selected, this will do all but the
 * last pass, and thus may take a great deal of time.
 *
 * Returns FALSE if suspended.  The return value need be inspected only if
 * a suspending data source is used.
 */
extern boolean  mtmplay_jpg_dec(j_decompress_ptr cinfo);

GLOBAL(boolean)
jpeg_start_decompress (j_decompress_ptr cinfo)
{
 JDIMENSION output_pitch;
	
    SkJpegDebugf("\n*******jpeg_start_decompress*******,out_color_space = %d\n",cinfo->out_color_space);
    SkJpegDebugf("2.1 image_width = %d, image_height = %d\n", cinfo->image_width,  cinfo->image_height);

#ifdef MTM_HWDEC
RESTART_DEC:
 // SkJpegDebugf("------restartdec %d---------",IS_HW_DECODE());
if (IS_HW_DECODE())
{
#ifdef SMALL_JPG_USE_SW
        if (((cinfo->image_width <640) && (cinfo->image_height<480))||(cinfo->out_color_space == JCS_RGB_565))
        {       
                    SkJpegDebugf("Small jpg Switch to SW mode\n");
            cinfo->fgforceswmode = TRUE;
            goto RESTART_DEC;
        }
#endif

#ifdef JPEG_FORCE_SW_FLOW
    cinfo->fgforceswmode=TRUE;
#endif    
    if(!fgJpgMutexInit)
    {            
        pthread_mutex_init(&JpgLibMutex, NULL);
        SkJpegDebugf("pthread_mutex_init JpgLibMutex ....\n");
        fgJpgMutexInit = TRUE;
    }
    pthread_mutex_lock(&JpgLibMutex);            
    SkJpegDebugf("JpgLibMutex lock....\n");
                    
    MEMZERO(&hwcinfo, SIZEOF(HWCINFO));
    pthread_cond_init(&JpgStopCond, NULL);
    pthread_cond_init(&JpgStopMutex, NULL);

      /* Hardwire it to "no scaling" */

      switch (cinfo->out_color_space) {
      case JCS_GRAYSCALE:
        cinfo->out_color_components = 1;
        break;
      case JCS_RGB:
        cinfo->out_color_components = 3;
        break;
      case JCS_RGB_565:
        cinfo->out_color_components = 3;
        break;
      case JCS_RGBA_8888:
        cinfo->out_color_components = 4;
        break;
      default:          /* else must be same colorspace as in file */
        cinfo->out_color_components = cinfo->num_components;
        break;
      }
      cinfo->output_components  = cinfo->out_color_components;
#ifndef MTIMAGE_READ_HEADER
    if(cinfo->scale_denom)
    {
        cinfo->output_width = cinfo->image_width / cinfo->scale_denom ;
        cinfo->output_height = cinfo->image_height / cinfo->scale_denom ;
    }
#endif
    //SkJpegDebugf("1.output_width = %d, output_height = %d\n", cinfo->output_width,  cinfo->output_height);
    //SkJpegDebugf("image_width = %d, image_height = %d, output_component =%d\n", cinfo->image_width,  cinfo->image_height, cinfo->out_color_components);

    switch(cinfo->out_color_space)
    {
        case JCS_RGBA_8888:
            Jpg_DesBytePerPixel = 4;
            Jpg_SrcBytePerPixel = 4;
            break;
        case JCS_RGB:
            Jpg_DesBytePerPixel = 3;            
            Jpg_SrcBytePerPixel = 4;            
            break;
        case JCS_RGB_565:
            Jpg_DesBytePerPixel = 2;                        
            Jpg_SrcBytePerPixel = 2;            
            break;
        default:
            break;
    }
#ifndef JPGMEM_IMPROVE
    output_pitch= (cinfo->output_width+0xf)&~0xf;
    if(!hwcinfo.image)
    {
        hwcinfo.image = malloc( output_pitch * cinfo->output_height * Jpg_SrcBytePerPixel * sizeof(UINTJ8));   // *4 for ARGB8888 buffer, YC422 to ARGB8888
        if (!hwcinfo.image)
        {
            SkJpegDebugf("allocate fail\n");
			pthread_mutex_unlock(&JpgLibMutex);
            return FALSE;
        }
        cinfo->input_iMCU_row = (JDIMENSION) hwcinfo.image;
    }
#endif

	
    if (mtmplay_jpg_dec(cinfo))
    {
        cinfo->output_scanline = 0;
        #ifndef JPGMEM_IMPROVE
        if(!hwcinfo.pimageRow)
        {
            hwcinfo.pimageRow = malloc( cinfo->output_width  * Jpg_DesBytePerPixel);
            SkJpegDebugf("pimageRow ptr = %x\n", (JDIMENSION)hwcinfo.pimageRow);
            if (!hwcinfo.pimageRow)
            {
                printf("pimageRow malloc fail\n");
				pthread_mutex_unlock(&JpgLibMutex);
                return -1;
            }
        }
        #endif

	#ifdef JPGMEM_IMPROVE
	 hwcinfo.image = cinfo->input_iMCU_row;
     //hwcinfo.pimageRow = cinfo->input_iMCU_row - (cinfo->output_width *  Jpg_DesBytePerPixel);
     hwcinfo.pimageRow = cinfo->input_iMCU_row + 0x7F8000; 
	#endif
	
        SkJpegDebugf("HW original image_width = %d, image_height = %d\n", cinfo->image_width,  cinfo->image_height);
        SkJpegDebugf("HW output_width = %d, output_height = %d,hwcinfo.image = %x %x\n", cinfo->output_width,  cinfo->output_height,hwcinfo.image,hwcinfo.pimageRow);
		pthread_mutex_unlock(&JpgLibMutex);

        return TRUE;
    }
    else
    {
        SkJpegDebugf("\n!!!-->HW Err.\n");
#if 1
        //return 0;

        /* Initialize marker processor so application can override methods
        * for COM, APPn markers before calling jpeg_read_header.
        */
        cinfo->marker_list = NULL;
        jinit_marker_reader(cinfo);

        /* And initialize the overall input controller. */
        jinit_input_controller(cinfo);

        /* OK, I'm ready */
        cinfo->global_state = DSTATE_START;
            
        cinfo->fgforceswmode=TRUE;
        pthread_mutex_unlock(&JpgLibMutex);

        jpeg_read_header(cinfo, TRUE);
        //jpeg_calc_output_dimensions(cinfo);
        goto RESTART_DEC;

        return TRUE;
		#else
		      return FALSE;
		#endif
    }
}
else
#endif//MTM_HWDEC
{
#ifdef JPEG_FORCE_HW_FLOW
    if((cinfo->out_color_space == JCS_RGB)||(cinfo->out_color_space == JCS_RGBA_8888))
    {
        printf("***cinfo->out_color_space=[%d], never sw mode\n");
        return 0;
    }
#endif

#ifdef MTM_HWDEC
        SkJpegDebugf("\n****-->SW MODE\n");
#ifndef ANDROID_READ
        cinfo->src->next_input_byte =  hwcinfo.gSOS_next_input_byte;
        cinfo->src->bytes_in_buffer =  hwcinfo.gSOS_bytes_in_buffer;
        SkJpegDebugf("4. Cinfo src buf addr= 0x%x: len=%lu \n", cinfo->src->next_input_byte, cinfo->src->bytes_in_buffer);
#endif
#endif
	
  if (cinfo->global_state == DSTATE_READY) {
    /* First call: initialize master control, select active modules */
    jinit_master_decompress(cinfo);
    if (cinfo->buffered_image) {
      /* No more work here; expecting jpeg_start_output next */
      cinfo->global_state = DSTATE_BUFIMAGE;
      return TRUE;
    }
    cinfo->global_state = DSTATE_PRELOAD;
  }
  if (cinfo->global_state == DSTATE_PRELOAD) {
    /* If file has multiple scans, absorb them all into the coef buffer */
    if (cinfo->inputctl->has_multiple_scans) {
#ifdef D_MULTISCAN_FILES_SUPPORTED
      for (;;) {
	int retcode;
	/* Call progress monitor hook if present */
	if (cinfo->progress != NULL)
	  (*cinfo->progress->progress_monitor) ((j_common_ptr) cinfo);
	/* Absorb some more input */
	retcode = (*cinfo->inputctl->consume_input) (cinfo);
	if (retcode == JPEG_SUSPENDED)
	  return FALSE;
	if (retcode == JPEG_REACHED_EOI)
	  break;
	/* Advance progress counter if appropriate */
	if (cinfo->progress != NULL &&
	    (retcode == JPEG_ROW_COMPLETED || retcode == JPEG_REACHED_SOS)) {
	  if (++cinfo->progress->pass_counter >= cinfo->progress->pass_limit) {
	    /* jdmaster underestimated number of scans; ratchet up one scan */
	    cinfo->progress->pass_limit += (long) cinfo->total_iMCU_rows;
	  }
	}
      }
#else
      ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif /* D_MULTISCAN_FILES_SUPPORTED */
    }
    cinfo->output_scan_number = cinfo->input_scan_number;
  } else if (cinfo->global_state != DSTATE_PRESCAN)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
  /* Perform any dummy output passes, and set up for the final pass */
  return output_pass_setup(cinfo);
}//if (!IS_HW_DECODE())
}

/*
 * Tile decompression initialization.
 * jpeg_read_header must be completed before calling this.
 */

GLOBAL(boolean)
jpeg_start_tile_decompress (j_decompress_ptr cinfo)
{
  if (cinfo->global_state == DSTATE_READY) {
    /* First call: initialize master control, select active modules */
    cinfo->tile_decode = TRUE;
    jinit_master_decompress(cinfo);
    if (cinfo->buffered_image) {
      cinfo->global_state = DSTATE_BUFIMAGE;
      return TRUE;
    }
    cinfo->global_state = DSTATE_PRELOAD;
  }
  if (cinfo->global_state == DSTATE_PRELOAD) {
    cinfo->output_scan_number = cinfo->input_scan_number;
  } else if (cinfo->global_state != DSTATE_PRESCAN)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
  /* Perform any dummy output passes, and set up for the final pass */
  return output_pass_setup(cinfo);
}


/*
 * Set up for an output pass, and perform any dummy pass(es) needed.
 * Common subroutine for jpeg_start_decompress and jpeg_start_output.
 * Entry: global_state = DSTATE_PRESCAN only if previously suspended.
 * Exit: If done, returns TRUE and sets global_state for proper output mode.
 *       If suspended, returns FALSE and sets global_state = DSTATE_PRESCAN.
 */

LOCAL(boolean)
output_pass_setup (j_decompress_ptr cinfo)
{
    printf("output_pass_setup\n");

  if (cinfo->global_state != DSTATE_PRESCAN) {
    /* First call: do pass setup */
    (*cinfo->master->prepare_for_output_pass) (cinfo);
    cinfo->output_scanline = 0;
    cinfo->global_state = DSTATE_PRESCAN;
  }
  /* Loop over any required dummy passes */
  while (cinfo->master->is_dummy_pass) {
#ifdef QUANT_2PASS_SUPPORTED
    /* Crank through the dummy pass */
    while (cinfo->output_scanline < cinfo->output_height) {
      JDIMENSION last_scanline;
      /* Call progress monitor hook if present */
      if (cinfo->progress != NULL) {
	cinfo->progress->pass_counter = (long) cinfo->output_scanline;
	cinfo->progress->pass_limit = (long) cinfo->output_height;
	(*cinfo->progress->progress_monitor) ((j_common_ptr) cinfo);
      }
      /* Process some data */
      last_scanline = cinfo->output_scanline;
      (*cinfo->main->process_data) (cinfo, (JSAMPARRAY) NULL,
				    &cinfo->output_scanline, (JDIMENSION) 0);
      if (cinfo->output_scanline == last_scanline)
	return FALSE;		/* No progress made, must suspend */
    }
    /* Finish up dummy pass, and set up for another one */
    (*cinfo->master->finish_output_pass) (cinfo);
    (*cinfo->master->prepare_for_output_pass) (cinfo);
    cinfo->output_scanline = 0;
#else
    ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif /* QUANT_2PASS_SUPPORTED */
  }
  /* Ready for application to drive output pass through
   * jpeg_read_scanlines or jpeg_read_raw_data.
   */
  cinfo->global_state = cinfo->raw_data_out ? DSTATE_RAW_OK : DSTATE_SCANNING;

  printf("output_pass_setup-done\n");
  return TRUE;
}


/*
 * Read some scanlines of data from the JPEG decompressor.
 *
 * The return value will be the number of lines actually read.
 * This may be less than the number requested in several cases,
 * including bottom of image, data source suspension, and operating
 * modes that emit multiple scanlines at a time.
 *
 * Note: we warn about excess calls to jpeg_read_scanlines() since
 * this likely signals an application programmer error.  However,
 * an oversize buffer (max_lines > scanlines remaining) is not an error.
 */

GLOBAL(JDIMENSION)
jpeg_read_scanlines (j_decompress_ptr cinfo, JSAMPARRAY scanlines,
		     JDIMENSION max_lines)
{
  JDIMENSION row_ctr;
#ifdef MTM_HWDEC  
if (IS_HW_DECODE())
{
   JDIMENSION output_pitch;

    int i, j;
    UINTJ8* tempBuffer = (UINTJ8*)scanlines[0];
	//printf("jpeg_read_scanlines[hwcinfo.image=%x][hwcinfo.pimageRow=%x][W=%x][H=%x][line=%x]\n", hwcinfo.image, hwcinfo.pimageRow, cinfo->output_width, cinfo->output_height, max_lines);

    if (!hwcinfo.image ||!hwcinfo.pimageRow||!scanlines || !tempBuffer)
    {
        SkJpegDebugf("->ERR jpeg_read_scanlines  return\n");
        cinfo->output_scanline = cinfo->output_height;
        return FALSE;
    }

    if ((scanlines == NULL) || (tempBuffer == NULL))
    {
        SkJpegDebugf("scanlines==NULL\n");
        return FALSE;
    }
    if (hwcinfo.image == NULL)
    {
        SkJpegDebugf("image==NULL\n");
        return FALSE;
    }
    if (hwcinfo.pimageRow == NULL)
    {
        SkJpegDebugf("pimageRow==NULL\n");
        return FALSE;
    }

	output_pitch= (cinfo->output_width+0xf)&~0xf;
	
    //skip A
    for (i = 0;i < cinfo->output_width;i++)
    {
        switch(cinfo->out_color_space)
        {
            case JCS_RGBA_8888:
#ifdef ANDROID_LIBJPEG_RZ_IOMMU
                *(tempBuffer + i*Jpg_DesBytePerPixel) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel+2);//R
                *(tempBuffer + i*Jpg_DesBytePerPixel + 1) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel + 1);//G
                *(tempBuffer + i*Jpg_DesBytePerPixel + 2) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel );//B
                *(tempBuffer + i*Jpg_DesBytePerPixel + 3) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel+3 );//A
#else
                *(tempBuffer + i*Jpg_DesBytePerPixel) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel);//R
                *(tempBuffer + i*Jpg_DesBytePerPixel + 1) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel + 1);//G
                *(tempBuffer + i*Jpg_DesBytePerPixel + 2) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel + 2);//B
                *(tempBuffer + i*Jpg_DesBytePerPixel + 3) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel+3 );//A
#endif
                break;
            case JCS_RGB:
                *(tempBuffer + i*Jpg_DesBytePerPixel) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel);//R
                *(tempBuffer + i*Jpg_DesBytePerPixel + 1) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel + 1);//G
                *(tempBuffer + i*Jpg_DesBytePerPixel + 2) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel + 2);//B         
                break;
            case JCS_RGB_565:
                *(tempBuffer + i*Jpg_DesBytePerPixel+1) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel+1); 
                *(tempBuffer + i*Jpg_DesBytePerPixel) = *(hwcinfo.image + cinfo->output_scanline * output_pitch * Jpg_SrcBytePerPixel + i * Jpg_SrcBytePerPixel);
                break;
            default:
                break;
        }

    }
	
	//SkJpegDebugf("jpeg_read_scanlines[scanlines[0]=%x],outputscanline = %d\n", (JSAMPROW) scanlines[0],cinfo->output_scanline);
	//row_ctr = 0;
    //memcpy((JSAMPROW) scanlines[0], (const void *)hwcinfo.pimageRow, sizeof(UINTJ8)*cinfo->output_width*max_lines*Jpg_DesBytePerPixel);
    row_ctr = 1;
    cinfo->output_scanline += max_lines;
}
else
#endif //#ifdef MTM_HWDEC
{

	#ifdef JPEG_FORCE_HW_FLOW
	return 0;
	#endif
	
	    int j;
	printf("jpeg_read_scanlines[line=%x][total lines=%x][color space=%x]\n", max_lines, cinfo->output_scanline, cinfo->out_color_space);

  if (cinfo->global_state != DSTATE_SCANNING)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
  if (cinfo->output_scanline >= cinfo->output_height) {
    WARNMS(cinfo, JWRN_TOO_MUCH_DATA);
    return 0;
  }

  /* Call progress monitor hook if present */
  if (cinfo->progress != NULL) {
    cinfo->progress->pass_counter = (long) cinfo->output_scanline;
    cinfo->progress->pass_limit = (long) cinfo->output_height;
    (*cinfo->progress->progress_monitor) ((j_common_ptr) cinfo);
  }

  /* Process some data */
  row_ctr = 0;
printf("sw jpeg_read_scanlines[scanlines[0]=%x][%x][%x]\n", (JSAMPROW) scanlines[0], scanlines, max_lines);
  (*cinfo->main->process_data) (cinfo, scanlines, &row_ctr, max_lines);
	printf("\n");

    for (j = 0;j < 32;j++)
    {
		printf("(%x)", *(scanlines[0]+j));
    }	
	printf("\n");
  cinfo->output_scanline += row_ctr;
 printf("sw jpeg_read_scanlines, cinfo->output_scanline=%x, row_ctr=%x\n",cinfo->output_scanline, row_ctr);
}
//#endif

printf("jpeg_read_scanlines, return row_ctr=%x\n", row_ctr);
  return row_ctr;
}
/*
 * Initialize the jpeg decoder to decompressing a rectangle with size of (width, height)
 * and its upper-left corner located at (start_x, start_y).
 * Align start_x and start_y to multiplies of iMCU width and height, respectively.
 * Also, the new reader position and sampled image size will be returned in
 * (start_x, start_y) and (width, height), respectively.
 */

GLOBAL(void)
jpeg_init_read_tile_scanline(j_decompress_ptr cinfo, huffman_index *index,
		     int *start_x, int *start_y, int *width, int *height)
{
  // Calculates the boundary of iMCU
  int lines_per_iMCU_row = cinfo->max_v_samp_factor * DCTSIZE;
  int lines_per_iMCU_col = cinfo->max_h_samp_factor * DCTSIZE;
  int row_offset = *start_y / lines_per_iMCU_row;
  int col_left_boundary = ((*start_x / lines_per_iMCU_col)
            / index->MCU_sample_size) * index->MCU_sample_size;
  int col_right_boundary =
                  jdiv_round_up(*start_x + *width, lines_per_iMCU_col);

  cinfo->coef->MCU_columns_to_skip =
      *start_x / lines_per_iMCU_col - col_left_boundary;

  *height = (*start_y - row_offset * lines_per_iMCU_row) + *height;
  *start_x = col_left_boundary * lines_per_iMCU_col;
  *start_y = row_offset * lines_per_iMCU_row;
  cinfo->image_width = jmin(cinfo->original_image_width,
          col_right_boundary * lines_per_iMCU_col) -
          col_left_boundary * lines_per_iMCU_col;
  cinfo->input_iMCU_row = row_offset;
  cinfo->output_iMCU_row = row_offset;

  // Updates JPEG decoder parameter
  jinit_color_deconverter(cinfo);
  jpeg_calc_output_dimensions(cinfo);
  jinit_upsampler(cinfo);
  (*cinfo->master->prepare_for_output_pass) (cinfo);
  if (cinfo->progressive_mode)
    (*cinfo->entropy->start_pass) (cinfo);
  else
    jpeg_decompress_per_scan_setup(cinfo);

  int sample_size = DCTSIZE / cinfo->min_DCT_scaled_size;

  *height = jdiv_round_up(*height, sample_size);
  *width = cinfo->output_width;
  cinfo->output_scanline = lines_per_iMCU_row * row_offset / sample_size;
  cinfo->inputctl->consume_input = cinfo->coef->consume_data;
  cinfo->inputctl->consume_input_build_huffman_index =
      cinfo->coef->consume_data_build_huffman_index;
  cinfo->entropy->index = index;
  cinfo->input_iMCU_row = row_offset;
  cinfo->output_iMCU_row = row_offset;
  cinfo->coef->MCU_column_left_boundary = col_left_boundary;
  cinfo->coef->MCU_column_right_boundary = col_right_boundary;
  cinfo->coef->column_left_boundary =
      col_left_boundary / index->MCU_sample_size;
  cinfo->coef->column_right_boundary =
      jdiv_round_up(col_right_boundary, index->MCU_sample_size);
}

/*
 * Read a scanline from the current position.
 *
 * Return the number of lines actually read.
 */

GLOBAL(JDIMENSION)
jpeg_read_tile_scanline (j_decompress_ptr cinfo, huffman_index *index,
        JSAMPARRAY scanlines)
{
  // Calculates the boundary of iMCU
  int lines_per_iMCU_row = cinfo->max_v_samp_factor * DCTSIZE;
  int lines_per_iMCU_col = cinfo->max_h_samp_factor * DCTSIZE;
  int sample_size = DCTSIZE / cinfo->min_DCT_scaled_size;
  JDIMENSION row_ctr = 0;

  if (cinfo->progressive_mode) {
    (*cinfo->main->process_data) (cinfo, scanlines, &row_ctr, 1);
  } else {
    if (cinfo->output_scanline % (lines_per_iMCU_row / sample_size) == 0) {
      // Set the read head to the next iMCU row
      int iMCU_row_offset = cinfo->output_scanline /
            (lines_per_iMCU_row / sample_size);
      int offset_data_col_position = cinfo->coef->MCU_column_left_boundary /
            index->MCU_sample_size;
      huffman_offset_data offset_data =
          index->scan[0].offset[iMCU_row_offset][offset_data_col_position];
      (*cinfo->entropy->configure_huffman_decoder) (cinfo, offset_data);
    }
    (*cinfo->main->process_data) (cinfo, scanlines, &row_ctr, 1);
  }

  cinfo->output_scanline += row_ctr;
  return row_ctr;
}

/*
 * Alternate entry point to read raw data.
 * Processes exactly one iMCU row per call, unless suspended.
 */

GLOBAL(JDIMENSION)
jpeg_read_raw_data (j_decompress_ptr cinfo, JSAMPIMAGE data,
		    JDIMENSION max_lines)
{
  JDIMENSION lines_per_iMCU_row;

  if (cinfo->global_state != DSTATE_RAW_OK)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
  if (cinfo->output_scanline >= cinfo->output_height) {
    WARNMS(cinfo, JWRN_TOO_MUCH_DATA);
    return 0;
  }

  /* Call progress monitor hook if present */
  if (cinfo->progress != NULL) {
    cinfo->progress->pass_counter = (long) cinfo->output_scanline;
    cinfo->progress->pass_limit = (long) cinfo->output_height;
    (*cinfo->progress->progress_monitor) ((j_common_ptr) cinfo);
  }

  /* Verify that at least one iMCU row can be returned. */
  lines_per_iMCU_row = cinfo->max_v_samp_factor * cinfo->min_DCT_scaled_size;
  if (max_lines < lines_per_iMCU_row)
    ERREXIT(cinfo, JERR_BUFFER_SIZE);

  /* Decompress directly into user's buffer. */
  if (! (*cinfo->coef->decompress_data) (cinfo, data))
    return 0;			/* suspension forced, can do nothing more */

  /* OK, we processed one iMCU row. */
  cinfo->output_scanline += lines_per_iMCU_row;
  return lines_per_iMCU_row;
}


/* Additional entry points for buffered-image mode. */

#ifdef D_MULTISCAN_FILES_SUPPORTED

/*
 * Initialize for an output pass in buffered-image mode.
 */

GLOBAL(boolean)
jpeg_start_output (j_decompress_ptr cinfo, int scan_number)
{
    printf("jpeg_start_output ");

  if (cinfo->global_state != DSTATE_BUFIMAGE &&
      cinfo->global_state != DSTATE_PRESCAN)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
  /* Limit scan number to valid range */
  if (scan_number <= 0)
    scan_number = 1;
  if (cinfo->inputctl->eoi_reached &&
      scan_number > cinfo->input_scan_number)
    scan_number = cinfo->input_scan_number;
  cinfo->output_scan_number = scan_number;
  /* Perform any dummy output passes, and set up for the real pass */
  return output_pass_setup(cinfo);
}


/*
 * Finish up after an output pass in buffered-image mode.
 *
 * Returns FALSE if suspended.  The return value need be inspected only if
 * a suspending data source is used.
 */

GLOBAL(boolean)
jpeg_finish_output (j_decompress_ptr cinfo)
{
  if ((cinfo->global_state == DSTATE_SCANNING ||
       cinfo->global_state == DSTATE_RAW_OK) && cinfo->buffered_image) {
    /* Terminate this pass. */
    /* We do not require the whole pass to have been completed. */
    (*cinfo->master->finish_output_pass) (cinfo);
    cinfo->global_state = DSTATE_BUFPOST;
  } else if (cinfo->global_state != DSTATE_BUFPOST) {
    /* BUFPOST = repeat call after a suspension, anything else is error */
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
  }
  /* Read markers looking for SOS or EOI */
  while (cinfo->input_scan_number <= cinfo->output_scan_number &&
	 ! cinfo->inputctl->eoi_reached) {
    if ((*cinfo->inputctl->consume_input) (cinfo) == JPEG_SUSPENDED)
      return FALSE;		/* Suspend, come back later */
  }
  cinfo->global_state = DSTATE_BUFIMAGE;
  return TRUE;
}

#endif /* D_MULTISCAN_FILES_SUPPORTED */
