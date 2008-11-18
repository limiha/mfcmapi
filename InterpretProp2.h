#pragma once

// Function to convert property tags to their names
// Free lpszExactMatch and lpszPartialMatches with MAPIFreeBuffer
HRESULT PropTagToPropName(ULONG ulPropTag, BOOL bIsAB, LPTSTR* lpszExactMatch, LPTSTR* lpszPartialMatches);

HRESULT PropNameToPropTag(LPCTSTR lpszPropName, ULONG* ulPropTag);
HRESULT PropTypeNameToPropType(LPCTSTR lpszPropType, ULONG* ulPropType);
LPTSTR GUIDToStringAndName(LPCGUID lpGUID);
void GUIDNameToGUID(LPCTSTR szGUID, LPCGUID* lpGUID);

LPCWSTR NameIDToPropName(LPMAPINAMEID lpNameID);

HRESULT InterpretFlags(const LPSPropValue lpProp, LPTSTR* szFlagString);

HRESULT InterpretFlags(const ULONG ulFlagName, const LONG lFlagValue, LPTSTR* szFlagString);
HRESULT InterpretFlags(const ULONG ulFlagName, const LONG lFlagValue, LPCTSTR szPrefix, LPTSTR* szFlagString);
CString AllFlagsToString(const ULONG ulFlagName,BOOL bHex);

// Uber property interpreter - given an LPSPropValue, produces all manner of strings
// All LPTSTR strings allocated with new, delete with delete[]
void InterpretProp(LPSPropValue lpProp, // optional property value
				   ULONG ulPropTag, // optional 'original' prop tag
				   LPMAPIPROP lpMAPIProp, // optional source object
				   LPMAPINAMEID lpNameID, // optional named property information to avoid GetNamesFromIDs call
				   BOOL bIsAB, // true if we know we're dealing with an address book property (they can be > 8000 and not named props)
				   LPTSTR* lpszNameExactMatches, // Built from ulPropTag & bIsAB
				   LPTSTR* lpszNamePartialMatches, // Built from ulPropTag & bIsAB
				   CString* PropType, // Built from ulPropTag
				   CString* PropTag, // Built from ulPropTag
				   CString* PropString, // Built from lpProp
				   CString* AltPropString, // Built from lpProp
				   LPTSTR* lpszSmartView, // Built from lpProp & lpMAPIProp
				   LPTSTR* lpszNamedPropName, // Built from ulPropTag & lpMAPIProp
				   LPTSTR* lpszNamedPropGUID, // Built from ulPropTag & lpMAPIProp
				   LPTSTR* lpszNamedPropDASL); // Built from ulPropTag & lpMAPIProp

enum MAPIStructType {
	stUnknown = 0,
	stTimeZoneDefinition,
	stTimeZone,
	stSecurityDescriptor,
	stExtendedFolderFlags,
	stAppointmentRecurrencePattern,
	stRecurrencePattern,
	stReportTag,
	stConversationIndex,
	stTaskAssigners,
	stGlobalObjectId,
	stOneOffEntryId,
	stEntryId,
};

void InterpretBinaryAsString(SBinary myBin, MAPIStructType myStructType, LPMAPIPROP lpMAPIProp, ULONG ulPropTag, LPTSTR* lpszResultString);

HRESULT GetLargeBinaryProp(LPMAPIPROP lpMAPIProp, ULONG ulPropTag, LPSPropValue* lppProp);

// ExtendedFlagStruct
// =====================
//   This structure specifies an Extended Flag
//
typedef struct
{
	BYTE Id;
	BYTE Cb;
	union
	{
		DWORD ExtendedFlags;
		GUID SearchFolderID;
		DWORD SearchFolderTag;
		DWORD ToDoFolderVersion;
	} Data;
	BYTE* lpUnknownData;
} ExtendedFlagStruct;

// ExtendedFlagsStruct
// =====================
//   This structure specifies an array of Extended Flags
//
typedef struct
{
	ULONG ulNumFlags;
	ExtendedFlagStruct* pefExtendedFlags;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} ExtendedFlagsStruct;

void ExtendedFlagsBinToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteExtendedFlagsStruct.
ExtendedFlagsStruct* BinToExtendedFlagsStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteExtendedFlagsStruct(ExtendedFlagsStruct* pefExtendedFlags);
// result allocated with new, clean up with delete[]
LPTSTR ExtendedFlagsStructToString(ExtendedFlagsStruct* pefExtendedFlags);

void SDBinToString(SBinary myBin, LPMAPIPROP lpMAPIProp, ULONG ulPropTag, LPTSTR* lpszResultString);

// TimeZoneStruct
// =====================
//   This is an individual description that defines when a daylight
//   savings shift, and the return to standard time occurs, and how
//   far the shift is.  This is basically the same as
//   TIME_ZONE_INFORMATION documented in MSDN, except that the strings
//   describing the names 'daylight' and 'standard' time are omitted.
//
typedef struct
{
	DWORD lBias; // offset from GMT
	DWORD lStandardBias; // offset from bias during standard time
	DWORD lDaylightBias; // offset from bias during daylight time
	WORD wStandardYear;
	SYSTEMTIME stStandardDate; // time to switch to standard time
	WORD wDaylightDate;
	SYSTEMTIME stDaylightDate; // time to switch to daylight time
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} TimeZoneStruct;

void TimeZoneToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteTimeZoneStruct.
TimeZoneStruct* BinToTimeZoneStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteTimeZoneStruct(TimeZoneStruct* ptzTimeZone);
// result allocated with new, clean up with delete[]
LPTSTR TimeZoneStructToString(TimeZoneStruct* ptzTimeZone);

// TZRule
// =====================
//   This structure represents both a description when a daylight.
//   savings shift occurs, and in addition, the year in which that
//   timezone rule came into effect.
//
typedef struct
{
	BYTE bMajorVersion;
	BYTE bMinorVersion;
	WORD wReserved;
	WORD wTZRuleFlags;
	WORD wYear;
	BYTE X[14];
	DWORD lBias; // offset from GMT
	DWORD lStandardBias; // offset from bias during standard time
	DWORD lDaylightBias; // offset from bias during daylight time
	SYSTEMTIME stStandardDate; // time to switch to standard time
	SYSTEMTIME stDaylightDate; // time to switch to daylight time
} TZRule;

// TimeZoneDefinitionStruct
// =====================
//   This represents an entire timezone including all historical, current
//   and future timezone shift rules for daylight savings time, etc.
//
typedef struct
{
	BYTE bMajorVersion;
	BYTE bMinorVersion;
	WORD cbHeader;
	WORD wReserved;
	WORD cchKeyName;
	LPWSTR szKeyName;
	WORD cRules;
	TZRule* lpTZRule;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} TimeZoneDefinitionStruct;

void TimeZoneDefinitionToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteTimeZoneDefinitionStruct.
TimeZoneDefinitionStruct* BinToTimeZoneDefinitionStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteTimeZoneDefinitionStruct(TimeZoneDefinitionStruct* ptzdTimeZoneDefinition);
// result allocated with new, clean up with delete[]
LPTSTR TimeZoneDefinitionStructToString(TimeZoneDefinitionStruct* ptzdTimeZoneDefinition);

// [MS-OXOCAL].pdf
// PatternTypeSpecificStruct
// =====================
//   This structure specifies the details of the recurrence type
//
typedef union
{
	DWORD WeekRecurrencePattern;
	DWORD MonthRecurrencePattern;
	struct
	{
		DWORD DayOfWeek;
		DWORD N;
	} MonthNthRecurrencePattern;
} PatternTypeSpecificStruct;

typedef struct
{
	DWORD ChangeHighlightSize;
	DWORD ChangeHighlightValue;
	LPBYTE Reserved;
} ChangeHighlightStruct;

// RecurrencePatternStruct
// =====================
//   This structure specifies a recurrence pattern.
//
typedef struct
{
	WORD ReaderVersion;
	WORD WriterVersion;
	WORD RecurFrequency;
	WORD PatternType;
	WORD CalendarType;
	DWORD FirstDateTime;
	DWORD Period;
	DWORD SlidingFlag;
	PatternTypeSpecificStruct PatternTypeSpecific;
	DWORD EndType;
	DWORD OccurrenceCount;
	DWORD FirstDOW;
	DWORD DeletedInstanceCount;
	DWORD* DeletedInstanceDates;
	DWORD ModifiedInstanceCount;
	DWORD* ModifiedInstanceDates;
	DWORD StartDate;
	DWORD EndDate;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} RecurrencePatternStruct;

// ExceptionInfoStruct
// =====================
//   This structure specifies an exception
//
typedef struct
{
	DWORD StartDateTime;
	DWORD EndDateTime;
	DWORD OriginalStartDate;
	WORD OverrideFlags;
	WORD SubjectLength;
	WORD SubjectLength2;
	LPSTR Subject;
	DWORD MeetingType;
	DWORD ReminderDelta;
	DWORD ReminderSet;
	WORD LocationLength;
	WORD LocationLength2;
	LPSTR Location;
	DWORD BusyStatus;
	DWORD Attachment;
	DWORD SubType;
	DWORD AppointmentColor;
} ExceptionInfoStruct;

// ExtendedExceptionStruct
// =====================
//   This structure specifies additional information about an exception
//
typedef struct
{
	ChangeHighlightStruct ChangeHighlight;
	DWORD ReservedBlockEE1Size;
	LPBYTE ReservedBlockEE1;
	DWORD StartDateTime;
	DWORD EndDateTime;
	DWORD OriginalStartDate;
	WORD WideCharSubjectLength;
	LPWSTR WideCharSubject;
	WORD WideCharLocationLength;
	LPWSTR WideCharLocation;
	DWORD ReservedBlockEE2Size;
	LPBYTE ReservedBlockEE2;
} ExtendedExceptionStruct;

// AppointmentRecurrencePatternStruct
// =====================
//   This structure specifies a recurrence pattern for a calendar object
//   including information about exception property values.
//
typedef struct
{
	RecurrencePatternStruct* RecurrencePattern;
	DWORD ReaderVersion2;
	DWORD WriterVersion2;
	DWORD StartTimeOffset;
	DWORD EndTimeOffset;
	WORD ExceptionCount;
	ExceptionInfoStruct* ExceptionInfo;
	DWORD ReservedBlock1Size;
	LPBYTE ReservedBlock1;
	ExtendedExceptionStruct* ExtendedException;
	DWORD ReservedBlock2Size;
	LPBYTE ReservedBlock2;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} AppointmentRecurrencePatternStruct;

void AppointmentRecurrencePatternToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteAppointmentRecurrencePatternStruct.
AppointmentRecurrencePatternStruct* BinToAppointmentRecurrencePatternStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteAppointmentRecurrencePatternStruct(AppointmentRecurrencePatternStruct* parpPattern);
// result allocated with new, clean up with delete[]
LPTSTR AppointmentRecurrencePatternStructToString(AppointmentRecurrencePatternStruct* parpPattern);

void RecurrencePatternToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteRecurrencePatternStruct.
RecurrencePatternStruct* BinToRecurrencePatternStruct(ULONG cbBin, LPBYTE lpBin, size_t* lpcbBytesRead);
void DeleteRecurrencePatternStruct(RecurrencePatternStruct* prpPattern);
// result allocated with new, clean up with delete[]
LPTSTR RecurrencePatternStructToString(RecurrencePatternStruct* prpPattern);

// [MS-OXOMSG].pdf
// ReportTagStruct
// =====================
//   This structure specifies a report tag for a mail object
//
typedef struct
{
	CHAR Cookie[9]; // 8 characters + NULL terminator
	DWORD Version;
	ULONG cbStoreEntryID;
	LPBYTE lpStoreEntryID;
	ULONG cbFolderEntryID;
	LPBYTE lpFolderEntryID;
	ULONG cbMessageEntryID;
	LPBYTE lpMessageEntryID;
	ULONG cbSearchFolderEntryID;
	LPBYTE lpSearchFolderEntryID;
	ULONG cbMessageSearchKey;
	LPBYTE lpMessageSearchKey;
	ULONG cchAnsiText;
	LPSTR lpszAnsiText;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} ReportTagStruct;

void ReportTagToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteReportTagStruct.
ReportTagStruct* BinToReportTagStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteReportTagStruct(ReportTagStruct* prtReportTag);
// result allocated with new, clean up with delete[]
LPTSTR ReportTagStructToString(ReportTagStruct* prtReportTag);

// [MS-OXOMSG].pdf
// ResponseLevelStruct
// =====================
//   This structure specifies the response levels for a conversation index
//
typedef struct
{
	BOOL DeltaCode;
	DWORD TimeDelta;
	BYTE Random;
	BYTE ResponseLevel;
} ResponseLevelStruct;

// ConversationIndexStruct
// =====================
//   This structure specifies a report tag for a mail object
//
typedef struct
{
	BYTE UnnamedByte;
	FILETIME ftCurrent;
	GUID guid;
	ULONG ulResponseLevels;
	ResponseLevelStruct* lpResponseLevels;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} ConversationIndexStruct;

void ConversationIndexToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteConversationIndexStruct.
ConversationIndexStruct* BinToConversationIndexStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteConversationIndexStruct(ConversationIndexStruct* pciConversationIndex);
// result allocated with new, clean up with delete[]
LPTSTR ConversationIndexStructToString(ConversationIndexStruct* pciConversationIndex);


// [MS-OXOTASK].pdf

// TaskAssignerStruct
// =====================
//   This structure specifies single task assigner
//
typedef struct
{
	DWORD cbAssigner;
	ULONG cbEntryID;
	LPBYTE lpEntryID;
	LPSTR szDisplayName;
	LPWSTR wzDisplayName;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} TaskAssignerStruct;

// TaskAssignersStruct
// =====================
//   This structure specifies an array of task assigners
//
typedef struct
{
	DWORD cAssigners;
	TaskAssignerStruct* lpTaskAssigners;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} TaskAssignersStruct;

void TaskAssignersToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteTaskAssignersStruct.
TaskAssignersStruct* BinToTaskAssignersStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteTaskAssignersStruct(TaskAssignersStruct* ptaTaskAssigners);
// result allocated with new, clean up with delete[]
LPTSTR TaskAssignersStructToString(TaskAssignersStruct* ptaTaskAssigners);

// GlobalObjectIdStruct
// =====================
//   This structure specifies a Global Object Id
//
typedef struct
{
	BYTE Id[16];
	WORD Year;
	BYTE Month;
	BYTE Day;
	FILETIME CreationTime;
	LARGE_INTEGER X;
	DWORD dwSize;
	LPBYTE lpData;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} GlobalObjectIdStruct;

void GlobalObjectIdToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteGlobalObjectIdStruct.
GlobalObjectIdStruct* BinToGlobalObjectIdStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteGlobalObjectIdStruct(GlobalObjectIdStruct* pgoidGlobalObjectId);
// result allocated with new, clean up with delete[]
LPTSTR GlobalObjectIdStructToString(GlobalObjectIdStruct* pgoidGlobalObjectId);

// OneOffEntryIdStruct
// =====================
//   This structure specifies a One-Off Entry Id
//
typedef struct
{
	DWORD dwFlags;
	BYTE ProviderUID[16];
	DWORD dwBitmask;
	union
	{
		struct
		{
			LPWSTR szDisplayName;
			LPWSTR szAddressType;
			LPWSTR szEmailAddress;
		} Unicode;
		struct
		{
			LPSTR szDisplayName;
			LPSTR szAddressType;
			LPSTR szEmailAddress;
		} ANSI;
	} Strings;
	size_t JunkDataSize;
	LPBYTE JunkData; // My own addition to account for unparsed data in persisted property
} OneOffEntryIdStruct;

void OneOffEntryIdToString(SBinary myBin, LPTSTR* lpszResultString);
// Allocates return value with new. Clean up with DeleteOneOffEntryIdStruct.
OneOffEntryIdStruct* BinToOneOffEntryIdStruct(ULONG cbBin, LPBYTE lpBin);
void DeleteOneOffEntryIdStruct(OneOffEntryIdStruct* pooeidOneOffEntryId);
// result allocated with new, clean up with delete[]
LPTSTR OneOffEntryIdStructToString(OneOffEntryIdStruct* pooeidOneOffEntryId);

void EntryIdToString(SBinary myBin, LPTSTR* lpszResultString);