#ifndef EQUITY_CURVE_H
#define EQUITY_CURVE_H

#include "sierrachart.h"



class EquityCurve
{


private:

	class intra_trade_extremes {

	public:
		
		float max_drawdown_{};

		float equity_peak_{};
		float equity_valley_{};

		void set_intratrade_mae(float mae, float cumulative_pnl);

		void set_intratrade_equity_peak(float mfe, float cumulative_pnl);

		void set_intratrade_max_drawdown(float mae, float cumulative_pnl);
	};




private:

	float cumulative_pnl_{};
	float max_drawdown_{};

	float equity_peak_{};
	float equity_valley_{};

	intra_trade_extremes it;


public:
	
	EquityCurve(){}
	
	// currently unused copy constructor, 
	// will be needed for planned optimization functionality
	// EquityCurve(const EquityCurve* eq);

	void set_trade_info(SCStudyInterfaceRef sc, float trade_pnl, int mae, int mfe);

private:

	void set_cumulative_pnl(float trade_pnl);

	void set_equity_peak_and_valley();

	void set_max_drawdown();

public:

	float get_cumulative_pnl() const;

	float get_max_drawdown() const;

	float get_equity_peak() const;

	float get_equity_valley() const;


	float get_intratrade_max_drawdown() const;

	float get_intratrade_equity_peak() const;

	float get_intratrade_equity_valley() const;

	void reset();







};

#endif