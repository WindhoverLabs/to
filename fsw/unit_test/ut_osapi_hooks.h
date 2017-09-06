
int32 Ut_OSAPI_QueueCreateHook(uint32 *queue_id, const char *queue_name, uint32 queue_depth,
                      uint32 data_size, uint32 flags);
int32 Ut_OSAPI_QueuePutHook(uint32 queue_id, const void *data, uint32 size, uint32 flags);
int32 Ut_OSAPI_QueueGetHook(uint32 queue_id, void *data, uint32 size, uint32 *size_copied, int32 timeout);
int32 Ut_OSAPI_QueueGetIdByName (uint32 *queue_id, const char *queue_name);
void Ut_OS_API_Clear(void);
