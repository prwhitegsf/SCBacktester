#include "utilities.h"


namespace printer {

	void num(SCStudyInterfaceRef sc, SCString message, double value) {

		SCString mess;
		mess.Format("%s %f at index : %i", message.GetChars(), value, sc.Index);
		sc.AddMessageToLog(mess, 1);
	}

	void num(SCStudyInterfaceRef sc, SCString message, double value, int testIndex) {

		if (sc.Index == testIndex) {
			SCString mess;
			mess.Format("%s %f at index : %i", message.GetChars(), value, sc.Index);
			sc.AddMessageToLog(mess, 1);
		}
	}

	void message(SCStudyInterfaceRef sc, SCString message, SCString value) {

		SCString mess;
		mess.Format("%s %s at index : %i", message.GetChars(), value.GetChars(), sc.Index);
		sc.AddMessageToLog(mess, 1);
	}

	void message(SCStudyInterfaceRef sc, SCString message, SCString value, int testIndex) {

		if (sc.Index == testIndex) {
			SCString mess;
			mess.Format("%s %s at index : %i", message.GetChars(), value.GetChars(), sc.Index);
			sc.AddMessageToLog(mess, 1);
		}
	}


}





namespace checkif {

	bool bar_open(SCStudyInterfaceRef sc, int& lastIndex) {

		bool newBar{ true };

		if (sc.Index == 0) {
			lastIndex = -1;
		}

		if (sc.Index == lastIndex) {
			newBar = false;
		}
		else {
			newBar = true; // just for the sake of clarity
			lastIndex = sc.Index;
		}

		return newBar;
	}

	bool bar_close(SCStudyInterfaceRef sc) {

		if (sc.GetBarHasClosedStatus() != BHCS_BAR_HAS_CLOSED) {
			return true;
		}
		else {
			return false;
		}

	}

	bool session_start(SCStudyInterfaceRef sc) {

		if (sc.BaseDateTimeIn[sc.Index].GetTimeInSeconds() <= sc.StartTime1) {
			return true;
		}
		else
			return false;

	}

	bool reload_chart(SCStudyInterfaceRef sc) {


		if (sc.IsFullRecalculation && sc.Index == 0)
			return true;
		else
			return false;

	}

}




namespace subgraph {

	float get_value_from_study(SCStudyInterfaceRef sc, int studyID, int studySg, int offset)
	{
		float result{};
		SCFloatArray StudyReference;
		if (sc.GetStudyArrayUsingID(studyID, studySg, StudyReference) > 0
			&& StudyReference.GetArraySize() > 0) {
			if (StudyReference[sc.Index - offset] != 0) {
				result = StudyReference[sc.Index - offset];
			}
			else {
				result = 0;
			}
		}//
		return(result);//
	}

	void ez_sg(SCStudyInterfaceRef sc, float valIn, int sgNum, SCString sgName) {

		SCString temp;
		temp.Format("%i %s", sgNum, sgName.GetChars());
		sc.Subgraph[sgNum].Name = temp;
		sc.Subgraph[sgNum][sc.Index] = valIn;
	}

}





namespace chart {

	SCString get_date_string_from_bar_index(SCStudyInterfaceRef sc ,int bar_index) {

		SCString startDate;
		int  Year, Month, Day;
		sc.BaseDateTimeIn[bar_index].GetDateYMD(Year, Month, Day);
		return startDate.Format("%d-%02d-%02d", Year, Month, Day);

	}

	SCString get_date_string_from_SCDateTime(SCDateTime date) {

		SCString startDate;
		int  Year, Month, Day;
		date.GetDateYMD(Year, Month, Day);
		return startDate.Format("%d-%02d-%02d", Year, Month, Day);



	}

	SCString get_date_string_from_int(int date)
	{
		SCDateTime Date;

		Date.SetDate(date);
		
		return get_date_string_from_SCDateTime(Date);
	}

	SCString get_HHMM_string_from_time_in_seconds(int time_in_seconds) {

		SCString timeString;
		int minutes = (time_in_seconds % 3600) / 60;
		int hours = time_in_seconds / 3600;
		timeString.Format("%02d:%02d", hours, minutes);
		return timeString;

	}

	int get_bar_period_in_minutes(SCStudyInterfaceRef sc) {

		n_ACSIL::s_BarPeriod BarPeriod;
		sc.GetBarPeriodParameters(BarPeriod);

		int barTime = 0;

		if (BarPeriod.ChartDataType == INTRADAY_DATA
			&& BarPeriod.IntradayChartBarPeriodType == IBPT_DAYS_MINS_SECS)
		{
			int SecondsPerBar = BarPeriod.IntradayChartBarPeriodParameter1;
			barTime = SecondsPerBar / 60;
		}

		return barTime;


	}

	



}

