/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef TCPSIAD_H
#define TCPSIAD_H

//TODO rewrite includes
#include <ns3/tcp-congestion-ops.h>
#include <ns3/log.h>
#include <ns3/type-id.h>
#include <ns3/tcp-socket-state.h>
#include <ns3/object.h>
#include <ns3/nstime.h>
#include <ns3/simulator.h>
#include <climits>
#include <algorithm>

namespace ns3 {

  class TcpSiad : public TcpNewReno
  {
  public:

    static TypeId GetTypeId();

    TcpSiad(uint32_t configNumRtt, uint32_t cwnd);

    TcpSiad(const TcpSiad& sock);

    virtual std::string GetName() const;

    virtual Ptr<TcpCongestionOps> Fork();

    virtual uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t segmentsAcked) override;

    virtual void IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) override;

    virtual void CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event) override;

    virtual void PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt) override;

  private:

    static const uint32_t default_num_rtt;    // default Num_RTT value
    static const uint32_t minCwnd; //min cnwd

    uint32_t config_num_rtt;     // configured Num_RTT value
    uint32_t configNumMs;	//Configured Num_MS value
    uint32_t curr_num_rtt;       // current calculated Num_RTT
    // based on minimum of num_rtt and num_ms
    // or config_num_rtt
    uint32_t alpha;			//increase by alpha every rtt in increase
    uint32_t incthresh;          // Linear Increment threshold  to enter Fast Increase phase
    // (target value after decrease based on max. cwnd)
    uint32_t prev_max_cwnd;      // estimated maximum cwnd  at previous congestion event

    uint32_t dec_cnt;            // number of additional decreases (for current congestion epoch)
    bool  min_delay_seen;     // state variable if the minimum delay was seen after a regular window reduction
    bool  increase_performed; // state variable if at least one increase was performed before new decrease
    uint32_t	snd_cwnd_cnt;	/* Linear increase counter		*/
    uint32_t	snd_cwnd_clamp; /* Do not allow snd_cwnd to grow above this */
    //Delays
    uint64_t prev_delay;         // delay value of previous sample (to filter out single outliers)
    uint64_t curr_delay;         // filtered current delay value
    uint64_t min_delay;          // absolute minimum delay
    uint64_t curr_min_delay;     // minimum delay since last congestion event
    uint64_t prev_min_delay1;    // previous min_delay values if
    uint64_t prev_min_delay2;  // monotonously increasing values
    uint64_t prev_min_delay3;    // due to measurement errors
    bool isStart;
  };
}
#endif
