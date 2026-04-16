#ifndef NUMPY_ENUMS_H
#define NUMPY_ENUMS_H

/* ============================================================
 * DType - データ型 → C#側で使用するように変更・飽くまでクライアント用
 * ============================================================ */
typedef enum SDType {/* C言語 */
    Bool,           /* bool */
    SByte,          /* int8_t */
    Byte,           /* uint8_t */
    Short,          /* int16_t */
    UShort,         /* uint16_t */
    Int,            /* int32_t */
    UInt,           /* uint32_t */
    Long,           /* int64_t */
    ULong,          /* uint64_t */
    Float,          /* float */
    Double,         /* double */
} SDType;

/* ============================================================
 * ORDER - 配列のメモリレイアウト順序
 * ============================================================ */
typedef enum ORDER {
    ANYORDER    = -1,   /* FortranかCのどちらか（既存のレイアウトに従う） */
    CORDER      =  0,   /* C順（行優先、最後の軸が最も速く変化） */
    FORTRANORDER=  1,   /* Fortran順（列優先、最初の軸が最も速く変化） */
    KEEPORDER   =  2    /* 入力配列のレイアウトを維持 */
} ORDER;

/* ============================================================
 * CLIPMODE - 範囲外インデックスの処理方法
 * ============================================================ */
typedef enum CLIPMODE {
    RAISE = 0,  /* 範囲外でエラーを発生させる */
    WRAP  = 1,  /* 範囲外をラップアラウンド */
    CLIP  = 2   /* 範囲外をクリップ（端の値に切り詰め） */
} CLIPMODE;

/* ============================================================
 * CORRELATEMODE - 相関・畳み込みのモード
 * ============================================================ */
typedef enum CORRELATEMODE {
    VALID = 0,  /* 完全にオーバーラップする部分のみ */
    SAME  = 1,  /* 入力と同じサイズの出力 */
    FULL  = 2   /* 部分的なオーバーラップを含む全出力 */
} CORRELATEMODE;

/* ============================================================
 * SEARCHSIDE - searchsortedの検索方向
 * ============================================================ */
typedef enum SEARCHSIDE {
    SEARCHLEFT  = 0,    /* 左端（最小インデックス）を返す */
    SEARCHRIGHT = 1     /* 右端（最大インデックス）を返す */
} SEARCHSIDE;

/* ============================================================
 * SORTKIND - ソートアルゴリズムの種類
 * ============================================================ */
typedef enum SORTKIND {
    QUICKSORT  = 0, /* クイックソート（不安定） */
    HEAPSORT   = 1, /* ヒープソート（不安定） */
    MERGESORT  = 2, /* マージソート（安定） */
    STABLESORT = 2  /* 安定ソート（MERGESORTの別名） */
} SORTKIND;

/* ============================================================
 * SELECTKIND - 部分ソートの種類
 * ============================================================ */
typedef enum SELECTKIND {
    INTROSELECT = 0 /* イントロセレクト */
} SELECTKIND;

/* ============================================================
 * CASTING - 型キャストの許容範囲
 * ============================================================ */
typedef enum CASTING {
    NO_CASTING       = 0,   /* キャスト不可（同一型のみ） */
    EQUIV_CASTING    = 1,   /* 等価な型間のみ（例: int32 <-> uint32） */
    SAFE_CASTING     = 2,   /* 安全なキャストのみ（データ損失なし） */
    SAME_KIND_CASTING= 3,   /* 同種の型間（例: float32 -> float64） */
    UNSAFE_CASTING   = 4,   /* 任意のキャストを許可 */
    SAME_VALUE_CASTING = 5  /* 同じ値を持つ型間のみ */
} CASTING;

/* ============================================================
 * COPYMODE - コピーの挙動
 * ============================================================ */
typedef enum COPYMODE {
    COPY_IF_NEEDED = 0, /* 必要な場合のみコピー */
    COPY_ALWAYS    = 1, /* 常にコピー */
    COPY_NEVER     = 2  /* コピーしない（できない場合はエラー） */
} COPYMODE;

/* ============================================================
 * ASTYPECOPYMODE - astype時のコピーの挙動
 * ============================================================ */
typedef enum ASTYPECOPYMODE {
    ASTYPECOPY_IF_NEEDED = 0,   /* 必要な場合のみコピー */
    ASTYPECOPY_ALWAYS    = 1    /* 常にコピー */
} ASTYPECOPYMODE;

/* ============================================================
 * DEVICE - 演算デバイス
 * ============================================================ */
typedef enum DEVICE {
    DEVICE_CPU = 0  /* CPU上での演算 */
} DEVICE;

/* ============================================================
 * ARRAY FLAGS - 配列フラグ（ビットフラグ）
 * ============================================================ */
typedef enum ARRAY_FLAGS {
    ARRAY_C_CONTIGUOUS    = 0x0001, /* C順で連続したメモリ */
    ARRAY_F_CONTIGUOUS    = 0x0002, /* Fortran順で連続したメモリ */
    ARRAY_OWNDATA         = 0x0004, /* データの所有権を持つ */
    ARRAY_FORCECAST       = 0x0010, /* 強制的に型キャスト */
    ARRAY_ENSURECOPY      = 0x0020, /* 必ずコピーを作成 */
    ARRAY_ENSUREARRAY     = 0x0040, /* 必ずndarrayに変換 */
    ARRAY_ELEMENTSTRIDES  = 0x0080, /* 要素単位のストライド */
    ARRAY_ALIGNED         = 0x0100, /* メモリアライメントあり */
    ARRAY_NOTSWAPPED      = 0x0200, /* バイトスワップなし */
    ARRAY_WRITEABLE       = 0x0400, /* 書き込み可能 */
    ARRAY_WRITEBACKIFCOPY = 0x2000, /* コピー時に書き戻し */
    ARRAY_WARN_ON_WRITE   = 0x4000  /* 書き込み時に警告 */
} ARRAY_FLAGS;

#endif /* NUMPY_ENUMS_H */