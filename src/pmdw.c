#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pmdw.h"

ushort little_endian_sig = 0x4949;
ushort tiff_sig = 0x002a;
uint header_offset = 0x0008;
uint cfa_data = 32803;

typedef uint16_t TIFF_SHORT;
typedef uint32_t TIFF_LONG;
typedef int32_t TIFF_SLONG;

struct tiff_header
{
  TIFF_SHORT endian_sig;
  TIFF_SHORT tiff_sig;
  TIFF_LONG ifd0_offset;
};

typedef struct _ifd_entry
{
  TIFF_SHORT tag;
  TIFF_SHORT type;
  TIFF_LONG count;
  TIFF_LONG value_offset;
} ifd_entry;

struct sub_ifd0
{
  TIFF_SHORT count;
  ifd_entry new_sub_file;
  ifd_entry image_width;
  ifd_entry image_height;
  ifd_entry bits_per_sample;
  ifd_entry compression;
  ifd_entry photometric_interpretation;
  ifd_entry strip_offsets;
  ifd_entry samples_per_pixel;
  ifd_entry rows_per_strip;
  ifd_entry strip_byte_counts;
  ifd_entry cfa_repeat_pattern_dim;
  ifd_entry cfa_pattern;
  ifd_entry black_level_repeat_dim;
  ifd_entry black_level;
  ifd_entry white_level;
  ifd_entry default_crop_origin;
  ifd_entry default_crop_size;
  TIFF_LONG next_ifd;
}__attribute__((__packed__));

struct ifd0
{
  TIFF_SHORT count;
  ifd_entry new_sub_file;
  ifd_entry image_width;
  ifd_entry image_height;
  ifd_entry bits_per_sample;
  ifd_entry compression;
  ifd_entry photometric_interpretation;
  ifd_entry make;
  ifd_entry model;
  ifd_entry strip_offsets;
  ifd_entry orientation;
  ifd_entry samples_per_pixel;
  ifd_entry rows_per_strip;
  ifd_entry strip_byte_counts;
  ifd_entry date_time;
  ifd_entry sub_ifds;
  ifd_entry fnumber;
  ifd_entry exif_ifd;
  ifd_entry tiff_ep_standard_id;
  ifd_entry dng_version;
  ifd_entry dng_backward_version;
  ifd_entry unique_camera_model;
  ifd_entry color_matrix_1; 
  ifd_entry color_matrix_2; 
  ifd_entry as_shot_white_xy;
  ifd_entry lens_info;
  ifd_entry dng_private_data;
  ifd_entry calibration_illuminant1;
  ifd_entry calibration_illuminant2;
  TIFF_LONG next_ifd;
}__attribute__((__packed__));

static struct tiff_header header = {
  .endian_sig = 0x4949,
  .tiff_sig = 0x002a,
};

static struct sub_ifd0 raw_ifd0 = {
  .new_sub_file = {
    .tag = TAG_NEW_SUB_FILE_TYPE,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .image_width = {
    .tag = TAG_IMAGE_WIDTH,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .image_height = {
    .tag = TAG_IMAGE_LENGTH,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .bits_per_sample = {
    .tag = TAG_BITS_PER_SAMPLE,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 16,
  },
  .compression = {
    .tag = TAG_COMPRESSION,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 1,
  },
  .photometric_interpretation = {
    .tag = TAG_PHOTOMETRIC_INTERPRETATION,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 32803,
  },
  .strip_offsets = {
    .tag = TAG_STRIP_OFFSETS,
    .type = TIFF_TYPE_LONG,
    .count = 0,
    .value_offset = 0,
  },
  .samples_per_pixel = {
    .tag = TAG_SAMPLES_PER_PIXEL,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 1,
  },
  .rows_per_strip = {
    .tag = TAG_ROWS_PER_STRIP,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .strip_byte_counts = {
    .tag = TAG_STRIP_BYTE_COUNTS,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .cfa_repeat_pattern_dim = {
    .tag = TAG_CFA_REPEAT_PATTERN_DIM,
    .type = TIFF_TYPE_SHORT,
    .count = 2,
    .value_offset = 131074,
  },
  .cfa_pattern = {
    .tag = TAG_CFA_PATTERN,
    .type = TIFF_TYPE_BYTE,
    .count = 4,
    .value_offset = 0x01020001,
  },
  .black_level_repeat_dim = {
    .tag = TAG_BLACK_LEVEL_REPEAT_DIM,
    .type = TIFF_TYPE_SHORT,
    .count = 2,
    .value_offset = 131074,
  },
  .black_level = {
    .tag = TAG_BLACK_LEVEL,
    .type = TIFF_TYPE_SHORT,
    .count = 4,
    .value_offset = 0,
  },
  .white_level = {
    .tag = TAG_WHITE_LEVEL,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 959,
  },
  .default_crop_origin = {
    .tag = TAG_DEFAULT_CROP_ORIGIN,
    .type = TIFF_TYPE_SHORT,
    .count = 2,
    .value_offset = 0,
  },
  .default_crop_size = {
    .tag = TAG_DEFAULT_CROP_SIZE,
    .type = TIFF_TYPE_SHORT,
    .count = 2,
    .value_offset = 0,
  },
  .next_ifd = 0,
};

struct exif_ifd
{
  TIFF_SHORT count;
  ifd_entry exposure_time;
  ifd_entry iso_speed;
  ifd_entry sensitivity_type;
  ifd_entry focal_length;
  TIFF_LONG next_ifd;
}__attribute__((__packed__));

static struct exif_ifd exif_ifd0 = {
  .exposure_time = {
    .tag = TAG_EXIF_EXPOSURE_TIME,
    .type = TIFF_TYPE_RATIONAL,
    .count = 1,
    .value_offset = 0,
  },
  .iso_speed = {
    .tag = TAG_EXIF_ISO_SPEED,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 0,
  },
  .sensitivity_type = {
    .tag = TAG_EXIF_SENSITIVITY_TYPE,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 3,
  },
  .focal_length = {
    .tag = TAG_EXIF_FOCAL_LENGTH,
    .type = TIFF_TYPE_RATIONAL,
    .count = 1,
    .value_offset = 0,
  },
  .next_ifd = 0,
};

static struct ifd0 preview_ifd0 = {
  .new_sub_file = {
    .tag = TAG_NEW_SUB_FILE_TYPE,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 1,
  },
  .image_width = {
    .tag = TAG_IMAGE_WIDTH,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .image_height = {
    .tag = TAG_IMAGE_LENGTH,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .bits_per_sample = {
    .tag = TAG_BITS_PER_SAMPLE,
    .type = TIFF_TYPE_SHORT,
    .count = 3,
    .value_offset = 0,
  },
  .compression = {
    .tag = TAG_COMPRESSION,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 1,
  },
  .photometric_interpretation = {
    .tag = TAG_PHOTOMETRIC_INTERPRETATION,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 2,
  },
  .make = {
    .tag = TAG_MAKE,
    .type = TIFF_TYPE_ASCII,
    .count = 0,
    .value_offset = 0,
  },
  .model = {
    .tag = TAG_MODEL,
    .type = TIFF_TYPE_ASCII,
    .count = 0,
    .value_offset = 0,
  },
  .strip_offsets = {
    .tag = TAG_STRIP_OFFSETS,
    .type = TIFF_TYPE_LONG,
    .count = 0,
    .value_offset = 0,
  },
  .orientation = {
    .tag = TAG_ORIENTATION,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 1,
  },
  .samples_per_pixel = {
    .tag = TAG_SAMPLES_PER_PIXEL,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 3,
  },
  .rows_per_strip = {
    .tag = TAG_ROWS_PER_STRIP,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .strip_byte_counts = {
    .tag = TAG_STRIP_BYTE_COUNTS,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .date_time = {
    .tag = TAG_DATE_TIME,
    .type = TIFF_TYPE_ASCII,
    .count = 0,
    .value_offset = 0,
  },
  .sub_ifds = {
    .tag = TAG_SUB_IFDS,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .fnumber = {
    .tag = TAG_FNUMBER,
    .type = TIFF_TYPE_RATIONAL,
    .count = 1,
    .value_offset = 0,
  },
  .exif_ifd = {
    .tag = TAG_EXIF_IFD,
    .type = TIFF_TYPE_LONG,
    .count = 1,
    .value_offset = 0,
  },
  .tiff_ep_standard_id = {
    .tag = TAG_TIFF_EP_STANDARD,
    .type = TIFF_TYPE_BYTE,
    .count = 4,
    .value_offset = 1,
  },
  .dng_version = {
    .tag = TAG_DNG_VERSION,
    .type = TIFF_TYPE_BYTE,
    .count = 4,
    .value_offset = 769,
  },
  .dng_backward_version = {
    .tag = TAG_DNG_BACKWARD_VERSION,
    .type = TIFF_TYPE_BYTE,
    .count = 4,
    .value_offset = 513,
  },
  .unique_camera_model = {
    .tag = TAG_UNIQUE_CAMERA_MODEL,
    .type = TIFF_TYPE_ASCII,
    .count = 0,
    .value_offset = 0,
  },
  .color_matrix_1 = { 
    .tag = TAG_COLORMATRIX_1, 
    .type = TIFF_TYPE_SRATIONAL, 
    .count = 9, 
    .value_offset = 0, 
  }, 
  .color_matrix_2 = { 
    .tag = TAG_COLORMATRIX_2, 
    .type = TIFF_TYPE_SRATIONAL, 
    .count = 9, 
    .value_offset = 0, 
  }, 
  .as_shot_white_xy = {
    .tag = TAG_AS_SHOT_WHITE_XY,
    .type = TIFF_TYPE_RATIONAL,
    .count = 2,
    .value_offset = 0,
  },
  .lens_info = {
    .tag = TAG_LENS_INFO,
    .type = TIFF_TYPE_RATIONAL,
    .count = 4,
    .value_offset = 0,
  },
  .dng_private_data = {
    .tag = TAG_PRIVATE_DATA,
    .type = TIFF_TYPE_BYTE,
    .count = 0,
    .value_offset = 0,
  },
  .calibration_illuminant1 = {
    .tag = TAG_ILLUMINANT_1,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 17,
  },
  .calibration_illuminant2 = {
    .tag = TAG_ILLUMINANT_2,
    .type = TIFF_TYPE_SHORT,
    .count = 1,
    .value_offset = 21,
  },
  .next_ifd = 0,
};

TIFF_LONG preview_image_width = 640;
TIFF_LONG preview_image_height = 480;
struct BitsPerSample
{
  TIFF_SHORT p1;
  TIFF_SHORT p2;
  TIFF_SHORT p3r;
};

void image_into_strips(TIFF_LONG width, TIFF_LONG height, TIFF_SHORT bits_per_sample, TIFF_SHORT samples_per_pixel,
		       TIFF_LONG* rows_per_strip, TIFF_LONG* offset_count, TIFF_LONG* strip_byte_count, TIFF_LONG* last_strip_byte_count)
{
  (*rows_per_strip) = (64.0 * 1024 * 8) / ((float)width * samples_per_pixel * bits_per_sample);
  (*offset_count) = ((float)height)/(*rows_per_strip);
  (*strip_byte_count) = ((*rows_per_strip) * width * samples_per_pixel * bits_per_sample)/8;
  (*last_strip_byte_count) = 
    ((bits_per_sample * width * height * samples_per_pixel)/8)
    -((*offset_count)*(*strip_byte_count));
  if((*last_strip_byte_count)==0)
  {
    (*last_strip_byte_count) = (*strip_byte_count);
    (*offset_count)--;
  }
}

int
write_image(FILE* output, guint8* data, guint max)
{
  //  g_print("written from %d %d\n", data, fwrite(data+i*(max/8), 8, max/8, output));  
  g_print("written from 0x%p %d\n", data, fwrite(data, 1, max, output));  
  if(ferror(output))
  {
    perror("fehler");
  }
  return max;
}

static struct BitsPerSample bits_per_sample = 
{8,8,8};
const char* make = "Nokia";
const char* model = "N900";
const char* unique_camera_model = "N900";
//const TIFF_LONG fnumber[] = {28,10};
//const TIFF_LONG exposure_time[] = {100,1000};
//const TIFF_LONG focal_length[] = {52,10};
const TIFF_LONG color_matrix1[] =
  {1828436,1048576,
   -649941,1048576,
   -296684,1048576,
   -239912,1048576,
   1060235,1048576,
   -191596,1048576,
   -777,1048576,
   238480,1048576,
   68187,1048576,
  };
const TIFF_LONG color_matrix2[] =
  {1569092,1048576,
   -548307,1048576,
   -279801,1048576,
   -324945,1048576,
   1252560,1048576,
   -11736,1048576,
   -18163,1048576,
   281176,1048576,
   450502,1048576,
  };

const TIFF_LONG as_white_shot[] =
  {348479,1048576,
   357605,1048576};
const TIFF_SHORT black_level [] =
  {25, 25, 12, 25,
  };

int export_image_data(raw_export_data_t* export_data)
{
  FILE* output = fopen(export_data->file_name, "wb");
  TIFF_LONG offset = 0;
  int i = 0;
  g_print("export to %s\n", export_data->file_name);
  preview_ifd0.count = (sizeof(preview_ifd0) - sizeof(TIFF_SHORT)) / sizeof(ifd_entry);
  raw_ifd0.count = (sizeof(raw_ifd0) - sizeof(TIFF_SHORT))/sizeof(ifd_entry);
  exif_ifd0.count = 4;
  header.ifd0_offset = sizeof(header)+(480*640*3+2592*1968*2);
  fwrite(&header, sizeof(header), 1, output);
  int written = write_image(output, export_data->preview_data, (640*480*3));
  printf("written %d\n", written);
  int written2 = write_image(output, export_data->raw_data, (2576*1960*2));
  guchar dummy = '\0';
  for(i=0;i<104192;++i)
  {
    fwrite(&dummy,1 , 1, output);
  }

  written2+=(2592*1968*2)-(2576*1960*2);
  printf("written %d\n", written2);
  offset+= written;
  offset+= written2;
  preview_ifd0.image_width.value_offset = preview_image_width;
  preview_ifd0.image_height.value_offset = preview_image_height;
  offset += sizeof(preview_ifd0) + sizeof(raw_ifd0) + sizeof(header) + sizeof(exif_ifd0);
  preview_ifd0.bits_per_sample.value_offset = offset;
  offset += sizeof(bits_per_sample);
  preview_ifd0.make.value_offset = offset;
  preview_ifd0.make.count = strlen(make)+1;
  preview_ifd0.model.count = strlen(model)+1;
  offset += strlen(make)+1;
  preview_ifd0.model.value_offset = offset;
  offset += strlen(model)+1;
  TIFF_LONG rows_per_strip = 0;
  TIFF_LONG offset_count = 0;
  TIFF_LONG last_strip_byte_count = 0;
  TIFF_LONG strip_byte_count = 0;
  image_into_strips(640, 480, 8, 3, &rows_per_strip, &offset_count, &strip_byte_count, &last_strip_byte_count);
  printf("rows_per_strip %d offset_count %d strip_byte_count %d last_strip_byte_count %d\n",
	 rows_per_strip, offset_count, strip_byte_count, last_strip_byte_count);
  TIFF_LONG raw_rows_per_strip = 0;
  TIFF_LONG raw_offset_count = 0;
  TIFF_LONG raw_last_strip_byte_count = 0;
  TIFF_LONG raw_strip_byte_count = 0;
  image_into_strips(2592, 1968, 16, 1, &raw_rows_per_strip, &raw_offset_count, &raw_strip_byte_count, &raw_last_strip_byte_count);
  printf("raw_rows_per_strip %d &raw_offset_count %d &raw_strip_byte_count %d &raw_last_strip_byte_count %d\n",
	 raw_rows_per_strip, raw_offset_count, raw_strip_byte_count, raw_last_strip_byte_count);
  preview_ifd0.strip_offsets.count = offset_count+1;
  preview_ifd0.strip_offsets.value_offset = offset;
  offset+= (offset_count+1) * sizeof(TIFF_LONG);
  preview_ifd0.rows_per_strip.value_offset = rows_per_strip;
  preview_ifd0.strip_byte_counts.count = offset_count+1;
  preview_ifd0.strip_byte_counts.value_offset = offset;
  offset+= (offset_count+1) * sizeof(TIFF_LONG);
  raw_ifd0.strip_offsets.count = raw_offset_count+1;
  raw_ifd0.strip_offsets.value_offset = offset;
  offset+= (raw_offset_count+1) * sizeof(TIFF_LONG);
  raw_ifd0.rows_per_strip.value_offset = raw_rows_per_strip;
  raw_ifd0.strip_byte_counts.count = raw_offset_count+1;
  raw_ifd0.strip_byte_counts.value_offset = offset;
  offset+= (raw_offset_count+1) * sizeof(TIFF_LONG);
  raw_ifd0.black_level.value_offset = offset;
  offset+=4*sizeof(TIFF_SHORT);
  preview_ifd0.date_time.count = strlen(export_data->camera_capture_data->date_time_stamp)+1;
  preview_ifd0.date_time.value_offset = offset;
  offset+=strlen(export_data->camera_capture_data->date_time_stamp)+1;
  preview_ifd0.sub_ifds.value_offset = (2592*1968*2)+(640*480*3)+sizeof(preview_ifd0)+sizeof(header) + sizeof(exif_ifd0);
  preview_ifd0.exif_ifd.value_offset = (2592*1968*2)+(640*480*3)+sizeof(preview_ifd0)+sizeof(header);
  preview_ifd0.fnumber.value_offset = offset;
  offset+=2*sizeof(TIFF_LONG);
  exif_ifd0.exposure_time.value_offset = offset;
  offset+=2*sizeof(TIFF_LONG);
  exif_ifd0.focal_length.value_offset = offset;
  offset+=2*sizeof(TIFF_LONG);
  exif_ifd0.iso_speed.value_offset = export_data->camera_capture_data->capture_iso_speed;
  preview_ifd0.unique_camera_model.count = strlen(unique_camera_model)+1;
  preview_ifd0.unique_camera_model.value_offset = offset;
  offset+=strlen(unique_camera_model)+1;
  preview_ifd0.color_matrix_1.value_offset = offset; 
  offset+=sizeof(color_matrix1); 
  preview_ifd0.color_matrix_2.value_offset = offset; 
  offset+=sizeof(color_matrix2); 
  preview_ifd0.as_shot_white_xy.value_offset = offset;
  offset+=sizeof(as_white_shot);
  preview_ifd0.lens_info.value_offset = offset;
  offset+=8*sizeof(TIFF_LONG);
  raw_ifd0.image_width.value_offset = 2592;
  raw_ifd0.image_height.value_offset = 1968;
  raw_ifd0.default_crop_origin.value_offset = 0x00040008;
  raw_ifd0.default_crop_size.value_offset =   0x07a80a10;//0x07a80a18;
  raw_ifd0.image_height.value_offset = 1968;
  fwrite(&preview_ifd0, sizeof(preview_ifd0), 1, output);
  fwrite(&exif_ifd0, sizeof(exif_ifd0), 1, output);
  fwrite(&raw_ifd0, sizeof(raw_ifd0), 1, output);
  fwrite(&bits_per_sample, sizeof(bits_per_sample), 1, output);
  fwrite(make, 1,strlen(make)+1, output);
  fwrite(model, strlen(model)+1,1, output);
  {
    int i;
    TIFF_LONG strip_offset = sizeof(header);
    for(i=0;i<offset_count;++i)
    {
      fwrite(&strip_offset, sizeof(TIFF_LONG), 1, output);
      strip_offset += strip_byte_count;
    }
    fwrite(&strip_offset, sizeof(TIFF_LONG), 1, output);
    strip_offset += last_strip_byte_count;
    for(i=0;i<offset_count;++i)
    {
      fwrite(&strip_byte_count, sizeof(TIFF_LONG), 1, output);
    }
    fwrite(&last_strip_byte_count, sizeof(TIFF_LONG), 1, output);
    {
      int i;
      strip_offset = sizeof(header)+(480*640*3);
      for(i=0;i<raw_offset_count;++i)
      {
	fwrite(&strip_offset, sizeof(TIFF_LONG), 1, output);
	strip_offset += raw_strip_byte_count;
      }
      fwrite(&strip_offset, sizeof(TIFF_LONG), 1, output);
      strip_offset += raw_last_strip_byte_count;
      for(i=0;i<raw_offset_count;++i)
      {
	fwrite(&raw_strip_byte_count, sizeof(TIFF_LONG), 1, output);
      }
      fwrite(&raw_last_strip_byte_count, sizeof(TIFF_LONG), 1, output);
    }
  }
  fwrite(black_level, sizeof(black_level), 1, output);
  g_print("date time %s\n", export_data->camera_capture_data->date_time_stamp);
fwrite(export_data->camera_capture_data->date_time_stamp, strlen(export_data->camera_capture_data->date_time_stamp)+1,1, output);
  fwrite(&export_data->camera_capture_data->capture_fnumber_n, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_fnumber_d, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_exposure_time_n, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_exposure_time_d, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_focal_length_n, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_focal_length_d, sizeof(TIFF_LONG), 1, output);
  fwrite(unique_camera_model, strlen(unique_camera_model)+1, 1, output);
  fwrite(color_matrix1,sizeof(color_matrix1),1,output); 
  fwrite(color_matrix2,sizeof(color_matrix2),1,output); 
  fwrite(as_white_shot,sizeof(as_white_shot),1,output);
  fwrite(&export_data->camera_capture_data->capture_focal_length_n, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_focal_length_d, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_focal_length_n, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_focal_length_d, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_aperture_n, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_aperture_d, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_aperture_n, sizeof(TIFF_LONG), 1, output);
  fwrite(&export_data->camera_capture_data->capture_aperture_d, sizeof(TIFF_LONG), 1, output);

  fclose(output);
  return 0;
}
