#ifndef _LOGGING_H__
#define _LOGGING_H__

#define LOGGING			1
#define LOG_TIMESTAMP	1

#define g_logger	Serial

#if LOGGING

#define FSEMIC    F(";")
#define LOGVAL_SENSOR(a, b, c, d, e, f) LOG11(a, FSEMIC, b, FSEMIC, c, FSEMIC, d, FSEMIC, e, FSEMIC, f)

#define		LOGBEGEOF(text)	BegEofLogger	__eof(F(text));

#define		LOGMEM(text)      /*showFreeMem(F(text))*/

// for printf debugging of test execution
#define TESTLOGF(x)			LOGF(x)
#define TESTLOGF2(a,b)		LOGF2(a,b)
#define TESTLOGF3(a,b,c)	LOGF3(a,b,c)
#define TESTLOGF4(a,b,c,d)	LOGF4(a,b,c,d)

#define LOGF(a)         LOG(F(a))
#define LOGF2(a,b)		LOG2(F(a),b)
#define LOGF3(a,b,c)	LOG3(F(a),b,c)
#define LOGF4(a,b,c,d)	LOG4(F(a),b,c,d)
#define LOGF5(a,b,c,d,e)	LOG5(F(a),b,c,d,e)
#define LOGF6(a,b,c,d,e,f)	LOG6(F(a),b,c,d,e,f)
#define LOGF7(a,b,c,d,e,f,g)	LOG7(F(a),b,c,d,e,f,g)
#define LOGF8(a,b,c,d,e,f,g,h)	LOG8(F(a),b,c,d,e,f,g,h)

#if LOG_TIMESTAMP
	#define LOG_PREFIX(a) \
	{ \
		/*_logger.printPrefix(); LOGMEM("");*/ \
		g_logger.print(millis()); g_logger.print("| "); \
		g_logger.print(a); \
	}
#else
	#define LOG_PREFIX(a) \
		{ \
			/*_logger.printPrefix(); LOGMEM("");*/ \ 
			g_logger.print(a); \
		}
#endif

#define LOG_SUFFIX \
	g_logger.println("");

#define LOG(a) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); LOG_SUFFIX; }

#define LOG2(a, b) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); LOG_SUFFIX; }

#define LOG3(a, b, c) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); LOG_SUFFIX; }

#define LOG4(a, b, c, d) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); LOG_SUFFIX; }

#define LOG5(a, b, c, d, e) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); LOG_SUFFIX; }

#define LOG6(a, b, c, d, e, f) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); g_logger.print(f); LOG_SUFFIX; }

#define LOG7(a, b, c, d, e, f, g) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); g_logger.print(f); g_logger.print(g); LOG_SUFFIX; }

#define LOG8(a, b, c, d, e, f, g, h) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); g_logger.print(f); g_logger.print(g); g_logger.print(h); LOG_SUFFIX; }

#define LOG9(a, b, c, d, e, f, g, h, i) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); g_logger.print(f); g_logger.print(g); g_logger.print(h); g_logger.print(i); LOG_SUFFIX; }
  
#define LOG10(a, b, c, d, e, f, g, h, i, j) \
  /*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); g_logger.print(f); g_logger.print(g); g_logger.print(h); g_logger.print(i); g_logger.print(j); LOG_SUFFIX; }

#define LOG11(a, b, c, d, e, f, g, h, i, j, k) \
/*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); g_logger.print(f); g_logger.print(g); g_logger.print(h); g_logger.print(i); g_logger.print(j); g_logger.print(k); LOG_SUFFIX; }

#define LOG12(a, b, c, d, e, f, g, h, i, j, k, l) \
/*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); g_logger.print(f); g_logger.print(g); g_logger.print(h); g_logger.print(i); g_logger.print(j); g_logger.print(k);  g_logger.print(l); LOG_SUFFIX; }

#define LOG13(a, b, c, d, e, f, g, h, i, j, k, l, m) \
/*if (g_logger.m_DEBUG) */{ LOG_PREFIX(a); g_logger.print(b); g_logger.print(c); g_logger.print(d); g_logger.print(e); g_logger.print(f); g_logger.print(g); g_logger.print(h); g_logger.print(i); g_logger.print(j); g_logger.print(k);  g_logger.print(l); g_logger.print(m); LOG_SUFFIX; }
#else

#define LOG(a) 
#define LOG2(a, b) 
#define LOG3(a, b, c) 
#define LOG4(a, b, c, d) 
#define LOG5(a, b, c, d, e) 
#define LOG6(a, b, c, d, e, f) 
#define LOG7(a, b, c, d, e, f, g) 
#define LOG8(a, b, c, d, e, f, g, h) 
#define LOG9(a, b, c, d, e, f, g, h, i) 
#define LOG10(a, b, c, d, e, f, g, h, i, j) 

#define LOGF(a)
#define LOGF2(a, b)
#define LOGF3(a, b, c)
#define LOGF4(a, b, c, d)
#define LOGF5(a, b, c, d, e)
#define LOGF6(a, b, c, d, e, f)
#define LOGF7(a, b, c, d, e, f, g)
#define LOGF8(a, b, c, d, e, f, g, h)
#define LOGF9(a, b, c, d, e, f, g, h, i)
#define LOGF10(a, b, c, d, e, f, g, h, i, j)

#define LOGVAL_SENSOR(a, b, c, d, e, f)

#endif // _DEBUG



class  BegEofLogger
{
	public:
	 BegEofLogger(const __FlashStringHelper *str)
	{
		_str= str;
		LOGF2("BEG: ", _str.c_str());
	}

	~ BegEofLogger()
	{
		LOGF2("EOF: ", _str.c_str());
	}
	
	private:
	String _str;
};

#endif // _LOGGING_H__
