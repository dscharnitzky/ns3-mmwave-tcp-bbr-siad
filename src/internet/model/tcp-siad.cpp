#include "tcp-siad.h"

namespace ns3 {


NS_LOG_COMPONENT_DEFINE("TcpSiad");

NS_OBJECT_ENSURE_REGISTERED (TcpSiad);

const uint32_t TcpSiad::default_num_rtt = 20;
const uint32_t TcpSiad::minCwnd = 2;

TypeId TcpSiad::GetTypeId()
{
	static TypeId tid = TypeId("ns3::TcpSiad")
		.SetParent<TcpNewReno>()
		.SetGroupName("Internet")
		.AddConstructor<TcpSiad>()
		.AddAttribute("ConfigNumRtt", "The configurued NumRTT value",
			UintegerValue(0), MakeUintegerAccessor(&TcpSiad::config_num_rtt), MakeUintegerChecker<uint32_t>())
		.AddAttribute("ConfigNumMs", "The configured NumMS value",
			UintegerValue(0), MakeUintegerAccessor(&TcpSiad::configNumMs), MakeUintegerChecker<uint32_t>())
		.AddAttribute("CurrNumRtt", "The current value of NumRTT",
			UintegerValue(0), MakeUintegerAccessor(&TcpSiad::curr_num_rtt), MakeUintegerChecker<uint32_t>())
		.AddAttribute("Alpha", "The calculated alpha value, used to increment the congestion window each RTT",
			UintegerValue(10), MakeUintegerAccessor(&TcpSiad::alpha), MakeUintegerChecker<uint32_t>())
		.AddAttribute("Incthresh", "The target congestion window size at each epoch",
			UintegerValue(10), MakeUintegerAccessor(&TcpSiad::incthresh), MakeUintegerChecker<uint32_t>())
		.AddAttribute("PreviousMaxCwnd", "The max estimated congestion window in the previous epoch",
			UintegerValue(10), MakeUintegerAccessor(&TcpSiad::prev_max_cwnd), MakeUintegerChecker<uint32_t>())
		.AddAttribute("DecCnt", "Additional Decreases counter at each epoch",
			UintegerValue(0), MakeUintegerAccessor(&TcpSiad::dec_cnt), MakeUintegerChecker<uint32_t>())
		.AddAttribute("MinDelaySeen", "True if min RTT could be measured in this epoch, false otherwise",
			BooleanValue(false), MakeBooleanAccessor(&TcpSiad::min_delay_seen), MakeBooleanChecker())
		.AddAttribute("IncreasePerformed", "True if increase was performed in this epoch, false otherwise",
			BooleanValue(false), MakeBooleanAccessor(&TcpSiad::increase_performed), MakeBooleanChecker())
		.AddAttribute("SendCwndCnt", "Counts the how much the congestion window was increased since last time",
			UintegerValue(0), MakeUintegerAccessor(&TcpSiad::snd_cwnd_cnt), MakeUintegerChecker<uint32_t>())
		.AddAttribute("SendCwndClamp", "Congestion window must not be greater than this",
			UintegerValue(666), MakeUintegerAccessor(&TcpSiad::snd_cwnd_clamp), MakeUintegerChecker<uint32_t>())
		.AddAttribute("PreviousDelay", "The previously measured delay",
			UintegerValue(INT_MAX), MakeUintegerAccessor(&TcpSiad::prev_delay), MakeUintegerChecker<uint32_t>())
		.AddAttribute("CurrentDelay", "Currently measured delay",
			UintegerValue(INT_MAX), MakeUintegerAccessor(&TcpSiad::curr_delay), MakeUintegerChecker<uint32_t>())
		.AddAttribute("MinDelay", "Measured min delay, reseted if delays are monotoniously increasing",
			UintegerValue(INT_MAX), MakeUintegerAccessor(&TcpSiad::min_delay), MakeUintegerChecker<uint32_t>())
		.AddAttribute("CurrentMinDelay", "Min delay in the present epoch",
			UintegerValue(INT_MAX), MakeUintegerAccessor(&TcpSiad::curr_min_delay), MakeUintegerChecker<uint32_t>())
		.AddAttribute("PreviousMinDelay1", "Used to detect monotonic increasing values in delays",
			UintegerValue(0), MakeUintegerAccessor(&TcpSiad::prev_min_delay1), MakeUintegerChecker<uint32_t>())
		.AddAttribute("PreviousMinDelay2", "Used to detect monotonic increasing values in delays",
			UintegerValue(0), MakeUintegerAccessor(&TcpSiad::prev_min_delay2), MakeUintegerChecker<uint32_t>())
		.AddAttribute("PreviousMinDelay3", "Used to detect monotonic increasing values in delays",
			UintegerValue(0), MakeUintegerAccessor(&TcpSiad::prev_min_delay3), MakeUintegerChecker<uint32_t>());
	return tid;
}

TcpSiad::TcpSiad(uint32_t configNumRtt = 0, uint32_t cwnd = 10) :
	TcpNewReno(),
	config_num_rtt(configNumRtt),
	configNumMs(0),
	curr_num_rtt(configNumRtt),
	alpha(cwnd),
	incthresh(cwnd),
	prev_max_cwnd(cwnd),
	dec_cnt(0),
	min_delay_seen(true),
	increase_performed(false),
	snd_cwnd_cnt(0),
	snd_cwnd_clamp(666),
	prev_delay(INT_MAX),
	curr_delay(INT_MAX),
	min_delay(INT_MAX),
	curr_min_delay(INT_MAX),
	prev_min_delay1(0),
	prev_min_delay2(0),
	prev_min_delay3(0),
	isStart(true)
{
	//NS_LOG_FUNCTION (this << configNumRtt << cwnd);
	configNumRtt == 0 ? curr_num_rtt = default_num_rtt : curr_num_rtt = configNumRtt;
	/*
	siad->prior_snd_una = tp->snd_una;*/
}

TcpSiad::TcpSiad(const TcpSiad & sock)
  : TcpNewReno (sock)
{
	//NS_LOG_FUNCTION (this << sock);
	config_num_rtt = sock.config_num_rtt;
	configNumMs = sock.configNumMs;
	curr_num_rtt = sock.curr_num_rtt;
	alpha = sock.alpha;
	incthresh = sock.incthresh;
	prev_max_cwnd = sock.prev_max_cwnd;
	dec_cnt = sock.dec_cnt;
	min_delay_seen = sock.min_delay_seen;
	increase_performed = sock.increase_performed;
	snd_cwnd_cnt = sock.snd_cwnd_cnt;
	snd_cwnd_clamp = sock.snd_cwnd_clamp;
	prev_delay = sock.prev_delay;
	curr_delay = sock.curr_delay;
	min_delay = sock.min_delay;
	curr_min_delay = sock.curr_min_delay;
	prev_min_delay1 = sock.prev_min_delay1;
	prev_min_delay2 = sock.prev_min_delay2;
	prev_min_delay3 = sock.prev_min_delay3;
	isStart = sock.isStart;
}

std::string
TcpSiad::GetName() const
{
	return "TcpSiad";
}

Ptr<TcpCongestionOps>
TcpSiad::Fork()
{
	return CopyObject<TcpSiad>(this);
}

void
TcpSiad::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event)
{
	//NS_LOG_FUNCTION (this << tcb << event);
	switch (event)
	{
	case TcpSocketState::TcpCAEvent_t::CA_EVENT_COMPLETE_CWR:
		curr_min_delay = INT_MAX;
		dec_cnt = 0;
		min_delay_seen = false;
		increase_performed = false;
		break;
	default:
		break;
	}
}

void
TcpSiad::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt)
{
	//NS_LOG_FUNCTION (this << tcb << segmentsAcked << rtt);
	NS_LOG_INFO("[PktsAcked called]");
	//Get delay and filter
	uint64_t delay = rtt.GetMilliSeconds();
	curr_delay = std::min(delay, prev_delay);
	prev_delay = delay;
	if (delay <= min_delay)
	{
		min_delay = delay;
		curr_min_delay = delay;
		min_delay_seen = true;
	}
	else if (delay <= curr_min_delay)
	{
		curr_min_delay = delay;
		//After some time we still see this minimun, so we "reset" the minimum
		if (tcb->GetCwndInSegments() > tcb->GetSsThreshInSegments() + alpha + 1)
		{
			min_delay = delay;
			min_delay_seen = true;
		}
	}
	NS_LOG_INFO("delay = " << delay <<" [PktsAcked]");
}

void
TcpSiad::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
	//NS_LOG_FUNCTION (this << tcb << segmentsAcked);
	NS_LOG_INFO("[IncreaseWindow called]");
	if(isStart)
	{
		isStart = false;
		tcb->m_ssThresh = tcb->m_segmentSize * 20;
	}
	uint32_t cwnd = tcb->GetCwndInSegments();
	uint32_t ssthresh = tcb->GetSsThreshInSegments();
	//If in Slow Start/Fast Increase, skip Additional Decrease
	if (cwnd > incthresh || cwnd < ssthresh)
	{
		min_delay_seen = true;
	}
	NS_LOG_INFO("cwnd = " << cwnd << " ssthresh = " << ssthresh << " alpha = " << alpha <<
		" [IncreaseWindow check to enter AddDec] (cwnd > ssthresh + alpha + 2)");
	//Additional Decrease, at least 1 RTT must elapse since last decrease.
	if (cwnd > ssthresh + alpha + 2 && !min_delay_seen && dec_cnt < curr_num_rtt - 1)
	{
		NS_LOG_INFO("entered Add Dec [IncreaseWindow AddDec]");
		dec_cnt++;
		snd_cwnd_cnt = 0;
		//ssthresh here should be previous rtt's cwnd (?)
		cwnd = (min_delay * ssthresh) / curr_delay;
		NS_LOG_INFO("cwnd = " << cwnd << "min_delay = " << min_delay << " curr_delay = " << curr_delay
			<< " [after (min_delay * ssthresh) / curr_delay]");
		if (cwnd > minCwnd)
		{
			NS_LOG_INFO("cwnd > minCwnd [IncreaseWindow AddDec]");
			uint32_t alphaNew = std::max((uint32_t)1u, (incthresh - cwnd) / (curr_num_rtt - dec_cnt - 1));
			NS_LOG_INFO("alphaNew = " << alphaNew << " incthresh = " << incthresh << " curr_num_rtt = "
				<< curr_num_rtt << " dec_cnt = " << dec_cnt
				<<" [IncreaseWindow AddDec] alphaNew = (incthresh - cwnd) / (curr_num_rtt - dec_cnt - 1)");
			uint32_t reduce = cwnd / (curr_num_rtt - dec_cnt);
			NS_LOG_INFO("reduce = " << reduce << " [IncreaseWindow AddDec] reduce = cwnd / (curr_num_rtt - dec_cnt)");
			if (reduce >= alphaNew)
			{
				NS_LOG_INFO("reduce >= alpha [IncreaseWindow AddDec]");
				//Recalculate alpha
				alpha = std::max((uint32_t)1u, (incthresh - cwnd) / (curr_num_rtt - dec_cnt));
				NS_LOG_INFO("alpha = " << alpha <<
					" [IncreaseWindow AddDec] alpha = std::max((uint32_t)1u, (incthresh - cwnd) / (curr_num_rtt - dec_cnt)");
				//Prevent underflow
				if (cwnd > reduce + minCwnd)
				{
					cwnd -= reduce;
					NS_LOG_INFO("cwnd = " << cwnd << " [IncreaseWindow AddDec] cwnd -= reduce");
				}
				else
				{
					cwnd = minCwnd;
					NS_LOG_INFO("cwnd set to min [IncreaeWindow AddDec]");
					min_delay_seen = true;
				}
			}
			else
			{
				NS_LOG_INFO("reduce < alpha [IncreaseWindow AddDec]");
				alpha = alphaNew;
				NS_LOG_INFO("alpha = " << alpha << " [IncreaseWindow AddDec alpha = alphaNew");
				//Prevent underflow
				if (cwnd > alphaNew + minCwnd)
				{
					cwnd -= alpha;
					NS_LOG_INFO("cwnd = " << cwnd << " [IncreaseWindow AddDec] cwnd -= alpha");
				}
				else
				{
					cwnd = minCwnd;
					NS_LOG_INFO("cwnd set to min [IncreaeWindow AddDec]");
					min_delay_seen = true;
				}
			}
		}
		else
		{
			NS_LOG_INFO("cwnd <= minCwnd [IncreaseWindow AddDec]");
			cwnd = minCwnd;
			NS_LOG_INFO("cwnd set to min [IncreaeWindow AddDec]");
			min_delay_seen = true;
			alpha = (incthresh - cwnd) / (curr_num_rtt - dec_cnt);
			NS_LOG_INFO("alpha = " << alpha << " incthresh = " << incthresh << " cwnd = " << cwnd << " curr_num_rtt = "
				<< curr_num_rtt << " dec_cnt = " << dec_cnt
				<< " [IncreaseWindow AddDec] alpha = (incthresh - cwnd) / (curr_num_rtt - dec_cnt)");
		}
		if (alpha > cwnd)
		{
			//Alpha is too big (would do more than double per rtt)
			//Kérdés: miért nincs benne a Kernel Kódban?
			NS_LOG_INFO("alpha > cwnd [IncreaseWindow AddDec]");
			NS_LOG_INFO("alpha = " << alpha << " [IncreaseWindow AddDec]");
			alpha = cwnd;
			NS_LOG_INFO("alpha = " << alpha << " [IncreaseWindow AddDec] alpha = cwnd");
			//No more Additional Decereases
			min_delay_seen = true;
		}
		if (alpha < 1) {
			//Increase by at least 1 packet per rtt
			alpha = 1;
		}
		//Finally set ssthresh
		ssthresh = cwnd - 1;
		NS_LOG_INFO("ssthresh = " << ssthresh << " [IncreaseWindow AddDec] ssthresh = cwnd - 1");
	}
	//Regular Increase
	else
	{
		NS_LOG_INFO("entered regular inc [IncreaseWindow RegInc]");
		snd_cwnd_cnt += segmentsAcked;
		NS_LOG_INFO("snd_cwnd_cnt = " << snd_cwnd_cnt << " segmentsAcked = " << segmentsAcked << "  [IncreaseWindow RegInc] snd_cwnd_cnt += segmentsAcked");
		//Set configured num_rtt
		if (config_num_rtt != 0)
		{
			curr_num_rtt = config_num_rtt;
			NS_LOG_INFO("curr_num_rtt = " << curr_num_rtt << " [IncreaseWindow RegInc] curr_num_rtt = config_num_rtt");
		}
		uint32_t next = std::max((uint32_t)1, cwnd / alpha);
		NS_LOG_INFO("next = " << next << " cwnd = " << cwnd << " alpha = " << alpha << " [IncreaseWindow RegInc] next = std::max((uint32_t)1, cwnd / alpha)");
		if (snd_cwnd_cnt >= next)
		{
			NS_LOG_INFO("snd_cwnd_nct >= next [IncreaseWindow RegInc]");
			uint32_t n = snd_cwnd_cnt / next;
			NS_LOG_INFO("n = " << n << " [IncreaseWindow RegInc] n = snd_cwnd_cnt / next");
			if (cwnd < snd_cwnd_clamp)
			{
				NS_LOG_INFO("cwnd < snd_cwnd_clamp [IncreaseWindow RegInc]");
				int32_t inc = std::min(segmentsAcked, std::min(n, snd_cwnd_clamp - cwnd));
				NS_LOG_INFO("inc = " << inc << " [IncreaseWindow RegInc] inc = std::min(segmentsAcked, std::min(n, snd_cwnd_clamp - cwnd))");
				cwnd += inc;
				NS_LOG_INFO("cwnd = " << cwnd << " [IncreaseWindow RegInc] cwnd += inc");
				increase_performed = true;
				NS_LOG_INFO("incthresh = " << incthresh << " ssthresh = " << ssthresh << " [IncreaseWindow RegInc]");
				NS_LOG_INFO("calculating alpha below [IncreaseWindow RegInc]");
				//Just entered Congestion Avoidence from Slow Start
				if (cwnd >= ssthresh && (cwnd - inc) < ssthresh && incthresh > ssthresh)
				{
					NS_LOG_INFO("just entered cong avoid from slow start");
					alpha = std::max((uint32_t)1u, (incthresh - ssthresh) / curr_num_rtt);
					NS_LOG_INFO("alpha = " << alpha << " [IncreaseWindow RegInc] alpha = std::max((uint32_t)1u, (incthresh - ssthresh) / curr_num_rtt)");
				}
				//From here we can expect incthresh to be greater than ssthresh
				//At or after incthresh, or at or after ssthresh but don't know incthresh
				else if (cwnd >= ssthresh && (cwnd - inc) < ssthresh && incthresh <= ssthresh
					|| cwnd >= incthresh && (cwnd - inc) < incthresh)
				{
					NS_LOG_INFO("reset to 1: just entered fast increase/no info on incthresh(after slow start) [IncreaseWindow RegInc]");
					//TODO check paper
					//Reset alpha to 1, becouse we just entered Fast Increase
					alpha = 1;
				}
				//Fast Increase
				else if (cwnd > incthresh && alpha < (cwnd / 2))
				{
					NS_LOG_INFO("in fast incerase for some time  [IncreaseWindow RegInc]");
					//Double increase rate / rtt but limit alpha to 1,5 * cwnd
					alpha += inc;
					NS_LOG_INFO("alpha = " << alpha << "  [IncreaseWindow RegInc] alpha += inc");
				}
				//Slow Start
				else if (cwnd < ssthresh)
				{
					NS_LOG_INFO("in slow start  [IncreaseWindow RegInc]");
					alpha = cwnd;
					NS_LOG_INFO("alpha = " << alpha << "  [IncreaseWindow RegInc] alpha = cwnd");
				}
				if (alpha < 1)
				{
					alpha = 1;
					NS_LOG_INFO("reset alpha to 1, it was < 1  [IncreaseWindow RegInc]");
				}
			}
			snd_cwnd_cnt -= n * next;
		}
	}
	if (cwnd < minCwnd)
	{
		NS_LOG_INFO("cwnd was < minCwnd  [IncreaseWindow]");
		cwnd = minCwnd;
	}
	if (ssthresh < minCwnd)
	{
		NS_LOG_INFO("ssthresh was < minCwnd  [IncreaseWindow]");
		ssthresh = minCwnd;
	}
	tcb->m_cWnd = cwnd * tcb->m_segmentSize;
	tcb->m_ssThresh = ssthresh * tcb->m_segmentSize;
	NS_LOG_INFO("cwnd = " << cwnd << " ssthresh = " << ssthresh << " [IncreaseWindow end]");
}

uint32_t
TcpSiad::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t segmentsAcked)
{
	//NS_LOG_FUNCTION(this << tcb << segmentsAcked);
	NS_LOG_INFO("[GetSsThresh called]");
	//Reset congestion counter
	snd_cwnd_cnt = 0;
	uint32_t ssthresh = tcb->GetSsThreshInSegments();
	NS_LOG_INFO("ssthresh = " << ssthresh << " [GetSsThresh]");
	uint32_t cwnd = tcb->GetCwndInSegments();
	NS_LOG_INFO("cwnd = " << cwnd << " [GetSsThresh]");
	uint32_t cwndMax = cwnd;
	//Congestion event occured about 1 rtt ago, so we need to calculate the cwnd then,
	//wich means substracting the alpha. We have 5 cases depending on what alfa was:
	if (increase_performed)
	{
		NS_LOG_INFO("calculate cwndMax according to where are we");
		NS_LOG_INFO("incthresh = " << incthresh << " cwnd = " << cwnd << " ssthresh = " << ssthresh
			<< " alpha = " << alpha << " curr_num_rtt = " << curr_num_rtt << " [GetSsThresh]");
		//Just enterted Fast Increase, so alpha was reset to 1
		if (alpha == 1 && cwnd >= incthresh && incthresh > ssthresh)
		{
			NS_LOG_INFO("in fast increase [GetSsThresh]");
			//Here alpha = 1, but we need previous, so calculate it
			cwndMax = cwnd - (incthresh - ssthresh) / curr_num_rtt;
		}
		//Max rate in Fast Increase (alpha = cwnd/2)
		else if (alpha >= (cwnd / 2) && cwnd > incthresh)
		{
			NS_LOG_INFO("max rate in fast increase [GetSsThresh]");
			cwndMax = cwnd - cwnd / 3;
		}
		//Kérdés: kernel kódban miért logikai VAGY-gyal választja el és nem logikai ÉS-sel?
		//Max rate in Slow Start (alpha = cwnd)
		else if (alpha >= cwnd && cwnd <= ssthresh)
		{
			NS_LOG_INFO("max rate in slow start [GetSsThresh]");
			cwndMax = cwnd / 2;
		}
		//Kérdés: Kernel Kódban miért hiányzik innen a Slow Start esetre ellenőrzés?
		//Feltételezés: ezért van az előbbinél ÉS helyett VAGY, de akkor rosszul számolja
		//az előző alphát Slow Startnál?
		//In Slow Start or Fast Increase for some time
		else if ((cwnd > incthresh || cwnd < ssthresh) && alpha != 1)
		{
			NS_LOG_INFO("in slow start or fast increase for some time [GetSsThresh]");
			//substract alpha/2, but at least 2
			cwndMax = cwnd - alpha / 2;
		}
		//In Adaptive Increase
		else
		{
			NS_LOG_INFO("adaptive increase [GetSsThresh]");
			cwndMax = cwnd - alpha;
		}
		NS_LOG_INFO("cwndMax = " << cwndMax << "  [GetSsThresh]");
	}
	//Detecting monotonically increasing min delays
	if (min_delay < prev_min_delay1 || min_delay < prev_min_delay2 || min_delay < prev_min_delay3)
	{
		//Smaller the some of the prev min delays, so it is not increasing, reset them
		prev_min_delay1 = 0;
		prev_min_delay2 = 0;
		prev_min_delay3 = 0;
	}
	//Set a value if it is not set (=0) and min delay is greater than the previous values.
	else if (min_delay > prev_min_delay1)
	{
		if (prev_min_delay1 == 0)
		{
			prev_min_delay1 = min_delay;
		}
		else if (prev_min_delay2 == 0)
		{
			prev_min_delay2 = min_delay;
		}
		else if (min_delay > prev_min_delay2)
		{
			if (prev_min_delay3 == 0)
			{
				prev_min_delay3 = min_delay;
			}
			else if (min_delay > prev_min_delay3)
			{
				//We set min delay to the smallest value, this will proc Additional Decrease
				min_delay = prev_min_delay1;
				NS_LOG_INFO("min delay update = [GetSsThresh]");
				//Resetting the other 2
				prev_min_delay2 = 0;
				prev_min_delay3 = 0;
			}
		}
	}
	uint32_t ssthreshNew = cwndMax;
	NS_LOG_INFO("min_delay = " << min_delay << " curr_delay = " << curr_delay << " [GetSsThresh]");
	//If we have info on delay
	if (min_delay != INT_MAX && curr_delay != 0)
	{
		//beta = min_delay / curr_delay
		ssthreshNew = (min_delay * cwndMax) / curr_delay;
		NS_LOG_INFO("ssthreshNew = " << ssthreshNew << " [GetSsThresh] ssthreshNew = (min_delay * cwndMax) / curr_delay");
	}
	else
	{
		//No info on delay
		ssthreshNew = cwndMax / 2;
		NS_LOG_INFO("ssthreshNew = " << ssthreshNew << " [GetSsThresh] ssthreshNew = cwndMax / 2");
	}
	//Check if not too small
	//Kérdés: Kernel Kódban mi értelme az OFFSETnek?
	if (ssthreshNew < minCwnd)
	{
		ssthreshNew = minCwnd;
		NS_LOG_INFO("ssthreshNew = minCwnd [GetSsThresh]");
	}
	//If configured, config Num_RTT
	if (config_num_rtt != 0)
	{
		//Kérdés: Socket Option?
		curr_num_rtt = config_num_rtt;
		NS_LOG_INFO("curr_num_rtt = " << curr_num_rtt << " [GetSsThresh] from config_num_rtt");
	}
	//Else if configured and we have info on delay, set to config Num_MS
	else if (configNumMs != 0 && min_delay != INT_MAX && curr_delay != 0)
	{
		uint32_t numRtt = configNumMs / ((curr_delay + min_delay) / 2);
		NS_LOG_INFO("numRtt = " << numRtt << " [GetSsThresh] numRtt = configNumMs / ((curr_delay + min_delay) / 2)");
		//Num_RTT is at least the default
		curr_num_rtt = std::max(numRtt, default_num_rtt);
		NS_LOG_INFO("curr_num_rtt = " << curr_num_rtt << " [GetSsThresh] urr_num_rtt = std::max(numRtt, default_num_rtt)");
	}
	//Else: nothing is configured, set to default
	else
	{
		curr_num_rtt = default_num_rtt;
		NS_LOG_INFO("curr_num_rtt = " << curr_num_rtt << " [GetSsThresh] from default_num_rtt");
	}
	int32_t trend = cwndMax - prev_max_cwnd;
	NS_LOG_INFO("tend = " << trend << " [GetSsThresh] trend = cwndMax - prev_max_cwnd");
	if (prev_max_cwnd < 2 * cwndMax)
	{
		incthresh = std::max(cwndMax + trend, ssthreshNew);
		NS_LOG_INFO("incthresh = " << incthresh << " [GetSsThresh] std::max(cwndMax + trend, ssthreshNew)");
	}
	else
	{
		//Here cwnd - trend would underflow, since cwnd is unsigned and trend is larger than it.
		incthresh = ssthreshNew;
		NS_LOG_INFO("incthresh = " << incthresh << " [GetSsThresh] incthresh = ssthresh");
	}
	
	alpha = std::max((uint64_t)1, (incthresh - ssthreshNew) / curr_delay);
	NS_LOG_INFO("alpha = " << alpha << " [GetSsThresh] alpha = std::max((uint64_t)1, (incthresh - ssthreshNew) / curr_delay)");
	prev_max_cwnd = cwndMax;
	NS_LOG_INFO("ssthreshNew = " << ssthreshNew << " [GetSsThresh]");
	return ssthreshNew * tcb->m_segmentSize;
}
}
