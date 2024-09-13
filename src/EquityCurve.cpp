#include "EquityCurve.h"


// Currently unused copy constructor

/*
EquityCurve::EquityCurve(const EquityCurve* eq)
	: cumulative_pnl_{ eq->cumulative_pnl_ },
	max_drawdown_{ eq->max_drawdown_ },
	equity_peak_{ eq->equity_peak_ },
	equity_valley_{ eq->equity_valley_ }
{
	it.equity_peak_ = eq->it.equity_peak_;
}
*/

void EquityCurve::set_trade_info(SCStudyInterfaceRef sc, float trade_pnl, int mae, int mfe) {

	// we have to set the mae / mfe (for intratrade equity peak/valley) BEFORE we update the cumulative pnl!
	it.set_intratrade_mae((sc.CurrencyValuePerTick * mae), cumulative_pnl_);
	it.set_intratrade_equity_peak((sc.CurrencyValuePerTick * mfe), cumulative_pnl_);
	it.set_intratrade_max_drawdown((sc.CurrencyValuePerTick * mae), cumulative_pnl_);

	// only update AFTER the intra_trade info
	set_cumulative_pnl(trade_pnl);

	// set AFTER cumulative pnl
	set_equity_peak_and_valley();

	// set LAST
	set_max_drawdown();


}


void EquityCurve::set_cumulative_pnl(float trade_pnl) {
	cumulative_pnl_ += trade_pnl;
}


void EquityCurve::set_equity_peak_and_valley() {

	// Peak/Valley
	if (cumulative_pnl_ > equity_peak_)
		equity_peak_ = cumulative_pnl_;
	else if (cumulative_pnl_ < equity_valley_)
		equity_valley_ = cumulative_pnl_;

}


void EquityCurve::set_max_drawdown() {
	// max drawdown, calculate last
	if (equity_peak_ - cumulative_pnl_ > max_drawdown_)
		max_drawdown_ = equity_peak_ - cumulative_pnl_;
}


float EquityCurve::get_cumulative_pnl() const {

	return cumulative_pnl_;

}


float EquityCurve::get_max_drawdown() const {

	return max_drawdown_;
}


float EquityCurve::get_equity_peak() const {

	return equity_peak_;

}


float EquityCurve::get_equity_valley() const {

	return equity_valley_;

}


float EquityCurve::get_intratrade_max_drawdown() const {

	return it.max_drawdown_;

}


float EquityCurve::get_intratrade_equity_peak() const {
	return it.equity_peak_;
}


float EquityCurve::get_intratrade_equity_valley() const {
	return it.equity_valley_;
}


void EquityCurve::intra_trade_extremes::set_intratrade_mae(float mae, float cumulative_pnl) {

	if (cumulative_pnl - mae < equity_valley_)
		equity_valley_ = cumulative_pnl - mae;

}


void EquityCurve::intra_trade_extremes::set_intratrade_equity_peak(float mfe, float cumulative_pnl) {

	if (cumulative_pnl + mfe > this->equity_peak_)
		this->equity_peak_ = cumulative_pnl + mfe;

}


void EquityCurve::intra_trade_extremes::set_intratrade_max_drawdown(float mae, float cumulative_pnl) {

	if (this->equity_peak_ - cumulative_pnl - mae > this->max_drawdown_)
		this->max_drawdown_ = this->equity_peak_ - cumulative_pnl - mae;

}


void  EquityCurve::reset() {

	// Figure out a better way to do this
	cumulative_pnl_ = equity_peak_ = equity_valley_ = max_drawdown_ = 0;
	it.equity_peak_ = it.equity_valley_ = it.max_drawdown_ = 0;

}