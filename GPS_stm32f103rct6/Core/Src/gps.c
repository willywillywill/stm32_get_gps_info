/*
 * gps.c
 *
 *  Created on: Feb 19, 2024
 *      Author: User
 */
#include "main.h"
#include "gps.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R (double)6371004
#define Latitude_flag 2
#define Longitude_flag 4
#define NS_flag 3
#define EW_flag 5
#define GPGGA_max_cnt 10 // 15
#define GPS_Data_Size 100

extern UART_HandleTypeDef huart1;

char GPGGA_data[100];
char GPS_data[100];

void DMM_to_DD(GPGGA_info *gps){
    const char *d = ",";
    char *p;
    int cnt=0;
    char *DMM_Latitude, *DMM_Longitude;
    char localion_NS,localion_EW;

    p = strtok(gps->GPGGA_data, d);
    while (p != NULL){

        switch (cnt) {
        	case Latitude_flag:
        		DMM_Latitude = p;
        		break;
        	case Longitude_flag:
                DMM_Longitude = p;
                break;
        	case NS_flag:
                localion_NS = p[0];
                break;
        	case EW_flag:
                localion_EW = p[0];
                break;
        }

        cnt++;
        p = strtok(NULL, d);
    }
    gps->cnt = cnt;
    gps->state = cnt > GPGGA_max_cnt;
    if (gps->state == OFF) // <15
    	return;

    gps->DMM_Latitude = DMM_Latitude;
    gps->DMM_Longitude = DMM_Longitude;
    gps->localion_EW = localion_EW;
    gps->localion_NS = localion_NS;

    char DMM_Latitude_d[3], DMM_Latitude_c[9];
    char DMM_Longitude_d[4], DMM_Longitude_c[9];

    strncpy(DMM_Latitude_d, gps->DMM_Latitude, 2);
    strncpy(DMM_Latitude_c, gps->DMM_Latitude+2, 8);
    strncpy(DMM_Longitude_d, gps->DMM_Longitude, 3);
    strncpy(DMM_Longitude_c, gps->DMM_Longitude+3, 8);

    gps->DD_Latitude = (double)(atoi(DMM_Latitude_d) + atof(DMM_Latitude_c) / 60.0);
    gps->DD_Longitude = (double)(atoi(DMM_Longitude_d) + atof(DMM_Longitude_c) / 60.0);
}


double toRadians(double a){
    return a*(M_PI/180.0);
}
double distance_2(double latitude1, double longitude1, double latitude2, double longitude2){
    double deltaLaiitude = toRadians(latitude2-latitude1);
    double deltaLongitude = toRadians(longitude2-longitude1);

    latitude1 = toRadians(latitude1);
    latitude2 = toRadians(latitude2);

    double a = pow(sin(deltaLaiitude/2),2)+
               cos(latitude1)* cos(latitude2)*
               pow(sin(deltaLongitude/2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = R * c;
    return d;
}
void get_gps_data(GPGGA_info *gps_ap){
	if(HAL_UART_Receive(&huart1, GPS_data, sizeof(GPS_data), 100)) {
		for(int i=0; i<GPS_Data_Size ; i++){
			if (GPS_data[i]=='G' & GPS_data[i+1]=='N' & GPS_data[i+3]=='G' & GPS_data[i+4]=='A'){
				int j=i;
				while(GPS_data[j] != '$' & j<sizeof(GPS_data))
					j++;
				if (j==sizeof(GPS_data)-1){
					break;
				} else{
					int y=0;
					for (int k=i ; k<j ; k++){
						GPGGA_data[y] = GPS_data[k];
						y++;
					}
				}
			}
		}
		//distance = distance_2(gps_ap.DD_Latitude, gps_ap.DD_Longitude, gps_sp.DD_Latitude, gps_sp.DD_Longitude);

		for (int i=0; i<sizeof(gps_ap->GPGGA_data) ; i++)
				gps_ap->GPGGA_data[i] = GPGGA_data[i];
			DMM_to_DD(gps_ap);
	}

	memset(GPS_data, 0, sizeof(GPS_data));
}
