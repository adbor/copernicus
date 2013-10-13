/* 
 * File:   copernicus.h
 * Author: tbabb
 *
 * Created on October 5, 2013, 10:35 PM
 */

//TODO: support GPS time
//TODO: add processOnePacket(); allow for blocking commands.
//TODO: add commands

#ifndef COPERNICUS_H
#define	COPERNICUS_H

#define CTRL_DLE 0x10
#define CTRL_ETX 0x03

// arduino doesn't support std::vector
// so today we will be violating the zero/one/infinity rule.
#define N_GPS_LISTENERS 16

#include "gpstype.h"
#include "Arduino.h"

class copernicus; // fwd decl

/***************************
 * Listener class          *
 ***************************/

class gps_listener {
public:
    virtual ~gps_listener();
    
    virtual void gps_event(ReportType type, copernicus *gps) = 0;
};

/***************************
 * copernicus class        *
 ***************************/

// - what does the fix time mean / how does it relate to GPS time?
//   - the fix time is the time at which the fix was acquired. it will generally
//     be a few seconds in the past. Use your sync'd current GPS time 
//     to figure out how that relates to "now".
// - how does the GPS time relate to the last/next PPS?
//   - reported GPS time is that of the last PPS. So at the next
//     PPS pulse, add 1 to the captured GPS time, and that's the current time.

class copernicus {
public:
    copernicus(int serial=0);
    
    void receive();
    
    void beginCommand(CommandID cmd);
    void writeDataBytes(const uint8_t *bytes, int n);
    int  readDataBytes(uint8_t *dst, int n);
    void endCommand();
    
    HardwareSerial  *getSerial();
    const PosFix&    getPositionFix() const;
    const VelFix&    getVelocityFix() const;
    const GPSStatus& getStatus() const;
    
    bool addListener(gps_listener *lsnr);
    void removeListener(gps_listener *lsnr);
    
private:
    
    void processReport(ReportType type);
    
    bool process_p_LLA_32();
    bool process_p_LLA_64();
    bool process_p_XYZ_32();
    bool process_p_XYZ_64();
    bool process_v_XYZ();
    bool process_v_ENU();
    bool process_GPSTime();
    bool process_health();
    bool process_addl_status();
    bool process_sbas_status();
    
    // todo: fix this busy wait.
    inline void blockForData() { while (m_serial->available() <= 0) {} }
    bool endReport();
    
    HardwareSerial *m_serial;
    PosFix    m_pfix;
    VelFix    m_vfix;
    GPSTime   m_time;
    GPSStatus m_status;
    gps_listener *m_listeners[N_GPS_LISTENERS];
    uint8_t m_n_listeners;
};

#endif	/* COPERNICUS_H */
