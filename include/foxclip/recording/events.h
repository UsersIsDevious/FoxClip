#pragma once
/* 文字列イベント名（payloadは JSON 文字列推奨） */
#define FOXCLIP_EVENT_RECORDER_STARTING   "recorder.starting"
#define FOXCLIP_EVENT_RECORDER_STARTED    "recorder.started"
#define FOXCLIP_EVENT_RECORDER_START_FAIL "recorder.start_failed"

/* 例: start_failed の payload (JSON)
   { "code": -4, "message": "backend error", "detail": "..." }
*/
