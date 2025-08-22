#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "foxclip/core/export.h"
#include "foxclip/recorder/error.h"

/* 将来拡張のためのフラグ */
typedef enum FoxclipRecorderStartFlags {
	FOXCLIP_RECORDER_START_NONE = 0,
	/* 例: 配信中でも録画を開始してよいか */
	FOXCLIP_RECORDER_START_ALLOW_IF_STREAMING = 1 << 0,
	/* 例: 既に録画中なら成功扱いにする (冪等) */
	FOXCLIP_RECORDER_START_IDEMPOTENT_OK = 1 << 1,
} FoxclipRecorderStartFlags;

/* 拡張可能な Options（size により後方互換） */
typedef struct FoxclipRecorderStartOptions {
	size_t size;              /* 必須: 構造体サイズ (= sizeof(FoxclipRecorderStartOptions)) */
	unsigned int flags;       /* FoxclipRecorderStartFlags のビット和 */
	const char *outputName;   /* 任意: 特定の出力(フェーダ名等)を明示。NULLならデフォルト */
	const char *pathOverride; /* 任意: 保存先パスを一時的に上書き（NULLで無効） */
	int waitUntilStartedMs;   /* 任意: started 確定まで待つ最大ミリ秒(0以下で待たない) */
	/* ここから下は将来予約領域。NULL/0 初期化必須。 */
	const char *profileName;     /* 予約: プロファイル切替を伴う開始 */
	const char *sceneCollection; /* 予約: シーンコレクション切替を伴う開始 */
	void *reserved0;
	void *reserved1;
} FoxclipRecorderStartOptions;

/* 推奨初期化マクロ */
#define FOXCLIP_RECORDER_START_OPTIONS_INIT \
  {                                         \
    sizeof(FoxclipRecorderStartOptions),    \
    FOXCLIP_RECORDER_START_NONE,            \
    NULL, /* outputName */                  \
    NULL, /* pathOverride */                \
    0,    /* waitUntilStartedMs */          \
    NULL, /* profileName */                 \
    NULL, /* sceneCollection */             \
    NULL, /* reserved0 */                   \
    NULL  /* reserved1 */                   \
  }

/* C 公開 API：成功=0、失敗=<0 (FoxclipRecorderError) */
FOXCLIP_EXTERN int FOXCLIP_CALL foxclipRecordStart(const FoxclipRecorderStartOptions *opts);

#ifdef __cplusplus
} /* extern "C" */
#endif
