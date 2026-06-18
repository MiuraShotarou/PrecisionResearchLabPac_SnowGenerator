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
    Long,           /* int64_t */    /* npy_intp */
    ULong,          /* uint64_t */
    Float,          /* float */
    Double,         /* double */
} SDType;

typedef enum OpType {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Less,
    Less_Equal,
    Greater,
    Greater_Equal,
    Equal,
    Not_Equal,
    Logical_And,
    Logical_Or,
    Logical_Xor,
    Logical_Not,
    Negative,
} OpType;

typedef enum PadModeType {
    Constant,    // 定数値でパディング（デフォルト：0）
    Edge,        // 端の値でパディング
    LinearRamp,  // 端の値から終端値への線形補間でパディング
    Maximum,     // 最大値でパディング
    Mean,        // 平均値でパディング
    Median,      // 中央値でパディング
    Minimum,     // 最小値でパディング
    Reflect,     // 端の値を軸に反転してパディング
    Symmetric,   // 端の値を含めて対称にパディング
    Wrap,        // 配列を循環させてパディング
    Empty,       // 未定義値でパディング
} PadModeType;

typedef enum ArrayType //不要の可能性
{
    NdArray,
	Multi,
    Jag
} ArrayType;

typedef enum SortKind
{
	
}

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
 * OVERLAP - 重複の可否
 * ============================================================ */
typedef enum {
    MEM_OVERLAP_NO      = 0,  // 重複なし
    MEM_OVERLAP_YES     = 1,  // 重複あり
    MEM_OVERLAP_UNKNOWN = 2,  // 不明
} mem_overlap_t;

typedef enum {
    NPY_METH_NO_FLOATINGPOINT_ERRORS = 1 << 0, // 浮動小数点エラーを発生させない
    NPY_METH_REQUIRES_PYAPI          = 1 << 1, // Python APIが必要（GIL解放不可）
    NPY_METH_IS_REORDERABLE          = 1 << 2, // 順序変更可能
    NPY_METH_SUPPORTS_UNALIGNED      = 1 << 3, // アライメントなしをサポート
} NPY_ARRAYMETHOD_FLAGS;

#endif /* NUMPY_ENUMS_H */