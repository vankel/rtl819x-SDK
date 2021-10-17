#include "librpc/gen_ndr/ndr_eventlog.h"
#ifndef __SRV_EVENTLOG__
#define __SRV_EVENTLOG__
NTSTATUS _eventlog_ClearEventLogW(pipes_struct *p, struct eventlog_ClearEventLogW *r);
NTSTATUS _eventlog_BackupEventLogW(pipes_struct *p, struct eventlog_BackupEventLogW *r);
NTSTATUS _eventlog_CloseEventLog(pipes_struct *p, struct eventlog_CloseEventLog *r);
NTSTATUS _eventlog_DeregisterEventSource(pipes_struct *p, struct eventlog_DeregisterEventSource *r);
NTSTATUS _eventlog_GetNumRecords(pipes_struct *p, struct eventlog_GetNumRecords *r);
NTSTATUS _eventlog_GetOldestRecord(pipes_struct *p, struct eventlog_GetOldestRecord *r);
NTSTATUS _eventlog_ChangeNotify(pipes_struct *p, struct eventlog_ChangeNotify *r);
NTSTATUS _eventlog_OpenEventLogW(pipes_struct *p, struct eventlog_OpenEventLogW *r);
NTSTATUS _eventlog_RegisterEventSourceW(pipes_struct *p, struct eventlog_RegisterEventSourceW *r);
NTSTATUS _eventlog_OpenBackupEventLogW(pipes_struct *p, struct eventlog_OpenBackupEventLogW *r);
NTSTATUS _eventlog_ReadEventLogW(pipes_struct *p, struct eventlog_ReadEventLogW *r);
NTSTATUS _eventlog_ReportEventW(pipes_struct *p, struct eventlog_ReportEventW *r);
NTSTATUS _eventlog_ClearEventLogA(pipes_struct *p, struct eventlog_ClearEventLogA *r);
NTSTATUS _eventlog_BackupEventLogA(pipes_struct *p, struct eventlog_BackupEventLogA *r);
NTSTATUS _eventlog_OpenEventLogA(pipes_struct *p, struct eventlog_OpenEventLogA *r);
NTSTATUS _eventlog_RegisterEventSourceA(pipes_struct *p, struct eventlog_RegisterEventSourceA *r);
NTSTATUS _eventlog_OpenBackupEventLogA(pipes_struct *p, struct eventlog_OpenBackupEventLogA *r);
NTSTATUS _eventlog_ReadEventLogA(pipes_struct *p, struct eventlog_ReadEventLogA *r);
NTSTATUS _eventlog_ReportEventA(pipes_struct *p, struct eventlog_ReportEventA *r);
NTSTATUS _eventlog_RegisterClusterSvc(pipes_struct *p, struct eventlog_RegisterClusterSvc *r);
NTSTATUS _eventlog_DeregisterClusterSvc(pipes_struct *p, struct eventlog_DeregisterClusterSvc *r);
NTSTATUS _eventlog_WriteClusterEvents(pipes_struct *p, struct eventlog_WriteClusterEvents *r);
NTSTATUS _eventlog_GetLogIntormation(pipes_struct *p, struct eventlog_GetLogIntormation *r);
NTSTATUS _eventlog_FlushEventLog(pipes_struct *p, struct eventlog_FlushEventLog *r);
void eventlog_get_pipe_fns(struct api_struct **fns, int *n_fns);
NTSTATUS rpc_eventlog_init(void);
#endif /* __SRV_EVENTLOG__ */
