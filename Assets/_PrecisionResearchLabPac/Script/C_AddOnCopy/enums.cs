public enum SDType
{
    Bool,
    SByte,
    Byte,
    Short,
    UShort,
    Int,
    UInt,
    Long,
    ULong,
    Float,
    Double,
}

public enum PadModeType
{
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
}