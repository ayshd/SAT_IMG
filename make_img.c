#include <stdio.h>
// Satellite-Derived PM2.5, 2013, at 35% RH [ug/m3]
// url: http://fizz.phys.dal.ca/~atmos/martin/?page_id=140

/////////////////////////////////////////////
char *f_asc   = "/media/sf_share/cmp_sat_soramame/asc/GlobalGWR_PM25_GL_201301_201312-RH35_NoDust_NoSalt-NoNegs.asc";
/*  image info
    0.01° × 0.01° w GWR adjustment [.asc.zip]
    ncols  36000
    nrows  12470
    xllcenter  -180.00
    yllcenter  -54.84
    cellsize  0.01
    NODATA_value  -999.9 */
/////////////////////////////////////////////
char *f_color = "./color.txt";
char *f_ppm  = "japan_gwr_001x001";

static int color[60][3];
static char buf[128];
static double latlon[19737][2];

FILE *fp_in, *fp_out;
int i, j, ct;
double lat, lon, asc;
char command[128];



//////////////////////////////////////////////////
//////////////////////////////////////////////////
// main
//////////////////////////////////////////////////
//////////////////////////////////////////////////
int main (void)
{
  // read color scale
  fp_in = fopen(f_color, "r");
  if (fp_in == NULL) {
    fprintf(stderr, "failure to open file: %s\n", f_color);
    return -1;
  }

  i = 0;
  while (fgets(buf, sizeof(buf), fp_in) != NULL) {
    sscanf(buf, "%d %d %d", &color[i][0], &color[i][1], &color[i][2]);
    i++;
  }

  fclose(fp_in);



  /////////////////////////
  // make ppm image Japan Area
  // 48.5N - 21.5N,  119E - 152E
  // output image
  // height 3300 pixels
  // width  2699 pixels
  /////////////////////////
  fp_in = fopen(f_asc, "r");
  if (fp_in == NULL) {
    fprintf(stderr, "failure to open file: %s\n", f_asc);
    return -1;
  }

  sprintf(command, "%s.ppm", f_ppm);
  fp_out = fopen(command, "w");
  if (fp_out == NULL) {
    fprintf(stderr, "failure to open file: %s\n", command);
    return -1;
  }

  fprintf(fp_out, "P3\n");
  fprintf(fp_out, "3300 2699\n");
  fprintf(fp_out, "255\n");

  // skip 6 rows
  for (i=0; i<6; i++) fgets(buf, sizeof(buf), fp_in);

  lat = 69.86;
  lon = -180;

  for (i=0; i<(36000*12470); i++) {
    fscanf(fp_in, "%lf ", &asc);

    // Japan area; 48.5N - 21.5N,  119E - 152E
    if (21.5 <= lat - 0.01 && lat <= 48.5 && 119 <= lon && lon + 0.01 <= 152) {

      //printf("%lf %lf\n", lat, lon);

      if (asc < 0 || 100 < asc) {
	fprintf(fp_out, "255 255 255\n");
      } else {
	fprintf(fp_out, "%d %d %d\n", color[59-(int)(59*(asc/100))][0], color[59-(int)(59*(asc/100))][1], color[59-(int)(59*(asc/100))][2]);
	//fprintf(fp_out, "%d %d %d\n", color[(int)(59*(asc/100))][0], color[(int)(59*(asc/100))][1], color[(int)(59*(asc/100))][2]);
      }
    }

    if (i%36000 == 0) {
      lat = lat - 0.01;
      lon = -180;
    } else {
      lon = lon + 0.01;
    }
    if (lat < 21.5) break;
  }

  fclose(fp_out);
  fclose(fp_in);

  // imagemagick required
  // convert ppm to png image
  sprintf(command, "convert %s.ppm %s.png", f_ppm, f_ppm);
  printf("%s\n", command);
  system(command);
  sprintf(command, "rm %s.ppm", f_ppm);
  printf("%s\n", command);
  system(command);

  return 0;
}
