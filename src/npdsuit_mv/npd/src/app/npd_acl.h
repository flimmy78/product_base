#ifndef __NPD_ACL_H__
#define __NPD_ACL_H__

#define NPD_TIME_RANGE_SIZE		32

void npd_acl_init
(
	void
);
int npd_acl_flow_to_port
(
	void
);

typedef struct
{
    unsigned char       arEther[6];
}ETHERADDR;

typedef enum
{
	TIME_RANGE_DAILY=0,
	TIME_RANGE_MONDAY,
	TIME_RANGE_TUESDAY,
	TIME_RANGE_WEDNESDAY,
	TIME_RANGE_THURDAY,
	TIME_RANGE_FRIDAY,
	TIME_RANGE_SATURDAY,
	TIME_RANGE_SUNDAY,
	TIME_RANGE_WEEKDAYS,
	TIME_RANGE_WEEKEND
}ACL_TIME_RANGE_TYPE;
typedef struct
{
	unsigned int		index;
	ACL_TIME_RANGE_TYPE timeType;
	unsigned int		startime[2];
	unsigned int		endtime[2];
}PERIODINFO_STC;


typedef struct
{
	char 		   timeName[NPD_TIME_RANGE_SIZE];
	unsigned int   timeId;
	unsigned int   abstartime[5];
	unsigned int   absendtime[5];
	unsigned int 	periodCount;
	PERIODINFO_STC periodTime[65];
	
}ACL_TIME_RANGE_STC;
typedef struct
{
	char 		   *timeName;
	unsigned int   timeId;
	unsigned int   abstartime[5];
	unsigned int   absendtime[5];	
	unsigned int 	periodCount;
}ACL_TIME_RANGE_SHOW;

#endif

