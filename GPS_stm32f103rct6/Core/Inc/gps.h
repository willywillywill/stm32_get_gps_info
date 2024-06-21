/*
 * gps.h
 *
 *  Created on: Feb 19, 2024
 *      Author: User
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_

typedef struct {
    char GPGGA_data[100];
    char localion_NS, localion_EW;
    char *DMM_Latitude, *DMM_Longitude;
    double DD_Latitude, DD_Longitude;
    int cnt;
    enum{
    	OFF = 0,
    	ON = 1
    }state;
}GPGGA_info;

typedef struct {
	double DD_Latitude, DD_Longitude;
}Next_GPS;

void DMM_to_DD(GPGGA_info *gps);
double toRadians(double a);
double distance_2(double latitude1, double longitude1, double latitude2, double longitude2);
void get_gps_data(GPGGA_info *gps_ap);


#endif /* INC_GPS_H_ */
