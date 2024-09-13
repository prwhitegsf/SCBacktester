#ifndef UTILITIES
#define UTILITIES
#include "sierrachart.h"



// utilities

namespace printer {

	void num(SCStudyInterfaceRef sc, SCString message, double value);
	void num(SCStudyInterfaceRef sc, SCString message, double value, int testIndex);

	void message(SCStudyInterfaceRef sc, SCString message, SCString value);
	void message(SCStudyInterfaceRef sc, SCString message, SCString value, int testIndex);

}


namespace checkif {

	bool bar_open(SCStudyInterfaceRef sc, int& lastIndex);
	bool bar_close(SCStudyInterfaceRef sc);

	bool session_start(SCStudyInterfaceRef sc);
	bool reload_chart(SCStudyInterfaceRef sc);

}


namespace chart {

	

	SCString get_date_string_from_bar_index(SCStudyInterfaceRef sc,int bar_index);
	
	SCString get_HHMM_string_from_time_in_seconds(int time_in_seconds);

	int get_bar_period_in_minutes(SCStudyInterfaceRef sc);

	SCString get_date_string_from_SCDateTime(SCDateTime date);

	SCString get_date_string_from_int(int date);
	

}



namespace subgraph {


	float get_value_from_study(SCStudyInterfaceRef sc, int studyID, int studySg, int offset);

	void ez_sg(SCStudyInterfaceRef sc, float valIn, int sgNum, SCString sgName);
}





#endif // !UTILITIES
