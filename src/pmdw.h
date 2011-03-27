#define TIFF_TYPE_BYTE 1
#define TIFF_TYPE_ASCII 2
#define TIFF_TYPE_SHORT 3
#define TIFF_TYPE_LONG 4
#define TIFF_TYPE_RATIONAL 5
#define TIFF_TYPE_SLONG 9
#define TIFF_TYPE_SRATIONAL 10

#define TAG_NEW_SUB_FILE_TYPE          254
#define TAG_IMAGE_WIDTH                256
#define TAG_IMAGE_LENGTH               257
#define TAG_BITS_PER_SAMPLE            258
#define TAG_COMPRESSION                259
#define TAG_PHOTOMETRIC_INTERPRETATION 262
#define TAG_IMAGE_DESCRPTION           270
#define TAG_MAKE                       271
#define TAG_MODEL                      272
#define TAG_STRIP_OFFSETS              273
#define TAG_ORIENTATION                274
#define TAG_SAMPLES_PER_PIXEL          277
#define TAG_ROWS_PER_STRIP             278
#define TAG_STRIP_BYTE_COUNTS          279
#define TAG_DATE_TIME                  306
#define TAG_SUB_IFDS                   330
#define TAG_FNUMBER                  33437
#define TAG_EXIF_IFD                 34665
#define TAG_TIFF_EP_STANDARD         37398
#define TAG_DNG_VERSION              50706
#define TAG_DNG_BACKWARD_VERSION     50707
#define TAG_UNIQUE_CAMERA_MODEL      50708
#define TAG_COLORMATRIX_1            50721
#define TAG_COLORMATRIX_2            50722
#define TAG_AS_SHOT_WHITE_XY         50729
#define TAG_LENS_INFO                50736
#define TAG_PRIVATE_DATA             50740
#define TAG_ILLUMINANT_1             50778
#define TAG_ILLUMINANT_2             50779
#define TAG_CFA_REPEAT_PATTERN_DIM   33421
#define TAG_CFA_PATTERN              33422
#define TAG_BLACK_LEVEL_REPEAT_DIM   50713
#define TAG_BLACK_LEVEL              50714
#define TAG_WHITE_LEVEL              50717
#define TAG_DEFAULT_CROP_ORIGIN      50719
#define TAG_DEFAULT_CROP_SIZE        50720
#define TAG_EXIF_EXPOSURE_TIME       33434
#define TAG_EXIF_ISO_SPEED           34855
#define TAG_EXIF_SENSITIVITY_TYPE    34864
#define TAG_EXIF_FOCAL_LENGTH        37386


#include "camera-interface.h"

typedef struct
{
  guint8* raw_data;
  guint raw_width;
  guint raw_height;
  guint8* preview_data;
  guint preview_width;
  guint preview_height;
  gchar* time_stamp;
  gchar* file_name;
  CameraCaptureData* camera_capture_data;
}raw_export_data_t;

int export_image_data(raw_export_data_t* export_data);
