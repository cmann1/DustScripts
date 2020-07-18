namespace date
{
	
	int days_from_civil(timedate@ date)
	{
		int y = date.year() + 1900;
		const int m = date.mon() + 1;
		const int d = date.mday();

		 y -= m <= 2 ? 1 : 0;
		const int era = (y >= 0 ? y : y-399) / 400;
		const int yoe = y - era * 400;      // [0, 399]
		const int doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
		const int doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
		return era * 146097 + doe - 719468;
	}

	int days_from_civil(int y, int m, int d)
	{
		y -= m <= 2 ? 1 : 0;
		const int era = (y >= 0 ? y : y-399) / 400;
		const int yoe = y - era * 400;      // [0, 399]
		const int doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
		const int doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
		return era * 146097 + doe - 719468;
	}

	void civil_from_days(int z, int &out y, int &out m, int &out d)
	{
		z += 719468;
		const int era = (z >= 0 ? z : z - 146096) / 146097;
		const int doe = z - era * 146097;          // [0, 146096]
		const int yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
		y = yoe + era * 400;
		const int doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
		const int mp = (5*doy + 2)/153;                                   // [0, 11]
		d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
		m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
	}
	
	void calculate_easter(const int year, int &out month, int &out day)
	{
		const int A = year % 19;
		const int B = year / 100;
		const int C = year % 100;
		const int D = B / 4;
		const int E = B % 4;
		const int G = (8 * B + 13) / 25;
		const int H = (19 * A + B - D - G + 15) % 30;
		const int M = (A + 11 * H) / 319;
		const int J = C / 4;
		const int K = C % 4;
		const int L = (2 * E + 2 * J - K - H + M + 32) % 7;
		const int N = (H - M + L + 90) / 25;
		const int P = (H - M + L + N + 19) % 32;
		
		month = N;
		day = P;
	}
	
}