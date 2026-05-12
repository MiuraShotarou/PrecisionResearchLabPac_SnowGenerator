using System;
using System.Linq;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Cysharp.Threading.Tasks;

/// <summary> _pointer のnullチェックを忘れずに記述すること </summary>
/// <summary> メソッドの第一引数にthisを付与すること </summary>
/// <summary> NdArrayを返すメソッドは破壊的操作なのか新規作成操作なのかを考慮して設計すること </summary>
/// <summary> NdArrayを引数に渡すメソッドは、かならずヒープメモリからの解放処理をC言語側に記述すること </summary>
/// <summary> C言語側で余分な大きさを持った配列を新規生成する場合は、不定値が入ってしまうことを念頭においてコーディングすること </summary>
/// <summary> overflow対策は一旦しなくて良い </summary>
/// <summary> dimensionsのlongはintにしたい </summary>
/// <summary> C言語側 np → ns にリネームしたい </summary>
/// <summary> NdArrayのコンストラクタに、ユーザーが要素を指定して初期化できる実装を追加する </summary>
/// <summary> IDisposableインターフェイスの実装を検討 </summary>
/// <summary> CSLanguageNativeクラス内でジェネリクスを使用しない書き方にリファクタしたい </summary>
/// <summary> だいぶ先の話だが、Add関数やRemove関数も追加したいな </summary>
namespace SnowflakeNative
{
    /// <summary> Collection is NdArray </summary>
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        private IntPtr _pointer; //DisPoseを実装すべき
        /// <summary> for client method </summary>
        public NdArray(long[] dimensions, char order = 'C')
        {
            int nd = dimensions.Length;
            int itemsize = Marshal.SizeOf(typeof(T));
            _pointer = ndarray_create(nd, dimensions, itemsize, order);
            
            if (_pointer == IntPtr.Zero)
            {
                throw new InvalidOperationException("ndarray_create failed.");
            }
        }
        // ----------------------------------------------------------------
        // Listに対応するコンストラクタ
        // ----------------------------------------------------------------
        /// <summary> 要素を直接指定して初期化 ( new NdArray<int> { 1, 2, 3 } ) </summary>
        public NdArray(IEnumerable<T> collection)
        {
            // TODO
        }
        /// <summary> C#配列から初期化 ( new NdArray<int>(array) ) </summary>
        public NdArray(T[] array)
        {
            // TODO
        }
        /// <summary> C#多次元配列から初期化 ( new NdArray<int>(array) ) </summary>
        public NdArray(Array array)
        {
            // TODO
        }
        /// <summary> INdArray </summary>
        IntPtr INdArray._pointer => this._pointer; //Indexerでのみ使用中

        /// <summary> client dispose </summary>
        public static void Dispose(NdArray<T> src) => CSDispose(src._pointer);

        /// <summary> for developer method </summary>
        private NdArray()
        {
            _pointer = IntPtr.Zero;
        }
        private static NdArray<T> Packing<T>(NdArray<T> src, IntPtr pointer) where T : unmanaged
        {
            src._pointer = pointer;
            return src;
        }

        /// <summary> NdArrayCopy </summary>
        public static NdArray<TSource> NdArrayCopy<TSource>(NdArray<TSource> src) where TSource : unmanaged => Packing(src, CSCopy(src._pointer)); //※コンストラクタと機能被り可能性
        /// <summary> Zeros </summary>
        public static NdArray<TResult> Zeros<TResult>(long[] size, double value) where TResult : unmanaged =>  Packing<TResult>(new NdArray<TResult>(), CSZeros<TResult>(size, value));
        /// <summary> Ones </summary>
        public static NdArray<TResult> Ones<TResult>(long[] size, double value) where TResult : unmanaged =>  Packing<TResult>(new NdArray<TResult>(), CSOnes<TResult>(size, value));
        /// <summary> Full </summary>
        public static NdArray<TResult> Full<TResult>(long[] size, double value) where TResult : unmanaged =>  Packing<TResult>(new NdArray<TResult>(), CSFull<TResult>(size, value));
        /// <summary> Arange </summary>
        public static NdArray<TResult> Arange<TResult>(int end, char order) where TResult : unmanaged => Packing<TResult>(new NdArray<TResult>(), CSArange<TResult>(0, end, 1, order));
        public static NdArray<TResult> Arange<TResult>(int start, int end, char order) where TResult : unmanaged => Packing<TResult>(new NdArray<TResult>(), CSArange<TResult>(start, end, 1, order));
        public static NdArray<TResult> Arange<TResult>(int start, int end, int step, char order) where TResult : unmanaged => Packing<TResult>(new NdArray<TResult>(), CSArange<TResult>(start, end, step, order));
        /// <summary> DArange </summary>
        public static NdArray<TResult> DArange<TResult>(double start, double end, double step, char order) where TResult : unmanaged => Packing<TResult>(new NdArray<TResult>(), CSDArange<TResult>(start, end, step, order));
        /// <summary> RandomChoice </summary>
        public static NdArray<TResult> RandomChoice<TResult>(NdArray<TResult> src, long[] size, bool replace = true, float[] p = null) where TResult : unmanaged => Packing<TResult>(new NdArray<TResult>(), CSRandomChoice<TResult>(src._pointer, size, replace, p));
        
        /// <summary> Reshape </summary>
        public static NdArray<TResult> Reshape<TResult>(NdArray<TResult> src, long[] size) where TResult : unmanaged => Packing(src, CSReshape(src._pointer, size));
        /// <summary> Where 相性が悪いのでパス </summary>
        
        /// <summary> Sum </summary>
        public static NdArray<TResult> Sum<TSource, TResult>(NdArray<TSource> src, int axis = -1, bool keepdims = false) where TSource : unmanaged where TResult : unmanaged => Packing<TResult>(new NdArray<TResult>(), CSSum<TSource, TResult>(src._pointer, axis, keepdims));
        public static TResult Sum<TSource, TResult>(NdArray<TSource> src) where TSource : unmanaged where TResult : unmanaged => CSSum<TSource, TResult>(src._pointer);
        
        /// <summary> Cast </summary>
        public static NdArray<TResult> Cast<TSource, TResult>(NdArray<TSource> src) where TSource : unmanaged where TResult : unmanaged => Packing<TResult>(new NdArray<TResult>(), CSArrayCast<TSource, TResult>(src._pointer));

        /// <summary> Pad </summary>
        public static NdArray<TResult> Pad<TResult>(NdArray<TResult> src, int pad_wdith, PadModeType mode, double value) where TResult : unmanaged => Packing<TResult>(new NdArray<TResult>(), CSPad<TResult>(src._pointer, pad_wdith, mode, value));
        /// <summary> Stack </summary> //引数に受け取る型をユーザー側で統一してもらう仕様にする。
        public static NdArray<TResult> Stack<TResult>(NdArray<TResult>[] srcArray, uint axis = 0) where TResult : unmanaged => Packing(new NdArray<TResult>(), CSStack<TResult>(srcArray.Select(arr => arr._pointer).ToArray(), axis));
        public static NdArray<TResult> VStack<TResult>(NdArray<TResult>[] srcArray, int axis = -1) where TResult : unmanaged => Packing(new NdArray<TResult>(), CSVStack<TResult>(srcArray.Select(arr => arr._pointer).ToArray(), srcArray.Length, axis = -1));
    }

    /// <summary> Have CSharp Relay Method </summary>
    public abstract partial class CSLanguageNative : CLanguageNative
    {
        /// <summary> dispose </summary>
        protected static void CSDispose(IntPtr pointer)
        {
            ndarray_free(pointer);
            pointer = IntPtr.Zero;
        }
        
        // 引数, 戻り値に NdArray<T>の使用を禁止
        protected static IntPtr CSCopy(IntPtr pointer) => ndarray_copy(pointer);
        protected static IntPtr CSFull<TResult>(long[] size, double value) where TResult : unmanaged
        {
            SDType restype = GenericsToSDType<TResult>();
            int size_nd = size.Length;
            IntPtr result = np_full(size, size_nd, value, restype);
            if (result == IntPtr.Zero) {
                throw new InvalidOperationException(GetErrorMessage());
            }
            return result;
        }
        protected static IntPtr CSArange<TResult>(int start, int end, int step, char order) where TResult : unmanaged
        {
            SDType resType = GenericsToSDType<TResult>();
            return np_arange(start, end, step, resType, order); //orderはC言語で処理するかFortranで処するかを指定する
        }
        protected static IntPtr CSArrayCast<TSource, TResult>(IntPtr pointer) where TSource : unmanaged where TResult : unmanaged
        {
            SDType srcType = GenericsToSDType<TSource>();
            SDType resType = GenericsToSDType<TResult>();
            // pointerをC言語側へ渡す
            IntPtr result = np_ndarray_cast(pointer, srcType, resType); //SDTypeでどのメソッドを呼び出すのか決めている。+ Source元の配列のポインタから具体的な型のついたポインタに変換する必要がある。それを、C言語側で行う。C#側のNdArrayは用意しなくて良い。
            if (result == IntPtr.Zero) {
                throw new InvalidOperationException(GetErrorMessage());
            }
            return result;
        }
        protected static IntPtr CSRandomChoice<TResult>(IntPtr pointer, long[] size, bool replace, float[] p) where TResult : unmanaged
        {
            SDType resType = GenericsToSDType<TResult>();
            int size_nd = size.Length, p_nd = p?.Length ?? 0;
            return np_random_choice(pointer, size, size_nd, replace, p, p_nd, resType);
        }
        /// <summary>  </summary>
        protected static IntPtr CSPad<TResult>(IntPtr pointer, int pad_wdith, PadModeType mode = PadModeType.Constant, double value = 0d) where TResult : unmanaged
        {
            SDType resType = GenericsToSDType<TResult>();
            return np_pad(pointer, pad_wdith, mode, value, resType);
        }

        protected static IntPtr CSConcatenate<TResult>(IntPtr[] pointerArray, int axis = 0) where TResult : unmanaged
        {
            if (pointerArray.Any(ptr => ptr == IntPtr.Zero)) {
                throw new ArgumentException(GetErrorMessage());
            }
            int array_count = pointerArray.Length;
            SDType resType = GenericsToSDType<TResult>();
            return np_concatenate(pointerArray, array_count, axis, resType);
        }
        /// <summary> 配列Aに新しい次元として配列Bを追加し、新規配列として返す </summary>
        protected static IntPtr CSStack<TResult>(IntPtr[] pointerArray, uint axis = 0) where TResult : unmanaged
        {
            SDType resType = GenericsToSDType<TResult>();
            int pointerCount = pointerArray.Length;
            return np_stack(pointerArray, pointerCount, axis, resType); //形状チェックはC言語側で行う
        }

        /// <summary> 配列を縦方向に順番に積み重ねる </summary>
        protected static IntPtr CSVstack<TResult>(IntPtr[] pointerArray) where TResult : unmanaged
        {
            int array_count = pointerArray.Length;
            return np_vstack(pointerArray, array_count);
        }
        protected static IntPtr CSHstack<TResult>(IntPtr[] pointerArray) where TResult : unmanaged
        {
            int array_count = pointerArray.Length;
            return np_hstack(pointerArray, array_count);
        }
        
        protected static IntPtr CSSqueeze<TResult>(IntPtr pointer) where TResult : unmanaged
        {
            var result = np_squeeze(pointer);
        }
        
        /// <summary> 配列での受け取り </summary>
        protected static IntPtr CSSum<TSource, TResult>(IntPtr pointer, int axis = -1, bool keepdims = false) where TSource : unmanaged where TResult : unmanaged
        {
            SDType srcType = GenericsToSDType<TSource>();
            SDType resType = GenericsToSDType<TResult>();
            return np_sum_return_array(pointer, srcType, resType, axis, keepdims);
        }
        /// <summary> スカラーでの受け取り </summary>
        protected static TResult CSSum<TSource, TResult>(IntPtr pointer) where TSource : unmanaged where TResult : unmanaged
        {
            SDType srcType = GenericsToSDType<TSource>();
            double result = np_sum_return_scalar(pointer, srcType);
            return (TResult)Convert.ChangeType(result, typeof(TResult));
        }
        protected static IntPtr CSReshape(IntPtr pointer, long[] size)
        {
            int size_nd = size.Length;
            return np_reshape(pointer, size, size_nd);
        }
        /// <summary> Resize </summary>
        protected static IntPtr CSResize<TResult>(IntPtr pointer, long[] size) where TResult : unmanaged
        {
            int size_nd = size.Length;
            IntPtr result = np_resize(pointer, size, size_nd);
            // if (result == IntPtr.Zero) {
            //     throw new InvalidOperationException(GetErrorMessage());
            // }
            return result;
        }
        /// <summary> NdArrayクラスにてNdメソッドとして実装 </summary>
        protected static int CSNdArrayNd(IntPtr pointer)
        {  
            return ndarray_nd(pointer);
        }
        /// <summary>  </summary> //戻り値をint[]に変更したい
        protected static long[] CSShape(IntPtr pointer)
        {
            var get = np_shape(pointer);
            int nd = ndarray_nd(pointer);
            // int[] result = new int[nd];
            long[] result = new long[nd];
            Marshal.Copy(get, result, 0 ,nd);
            return result;
        }

        protected static IntPtr CSTranspose(IntPtr pointer, long[] size)
        {
            // size の例外条件洗い出し
            return np_transpose(pointer, size);
        }
        /// <summary> 便利系 </summary>
        protected static SDType GenericsToSDType<T>() where T : unmanaged
        {
            if      (typeof(T) == typeof(bool))   return SDType.Bool;
            else if (typeof(T) == typeof(sbyte))  return SDType.SByte;
            else if (typeof(T) == typeof(byte))   return SDType.Byte;
            else if (typeof(T) == typeof(short))  return SDType.Short;
            else if (typeof(T) == typeof(ushort)) return SDType.UShort;
            else if (typeof(T) == typeof(int))    return SDType.Int;
            else if (typeof(T) == typeof(uint))   return SDType.UInt;
            else if (typeof(T) == typeof(long))   return SDType.Long;
            else if (typeof(T) == typeof(ulong))  return SDType.ULong;
            else if (typeof(T) == typeof(float))  return SDType.Float;
            else if (typeof(T) == typeof(double)) return SDType.Double;
            else throw new NotSupportedException($"Unsupported type: {typeof(T)}");
        }
        protected static SDType TypeToSDType(Type t)
        {
            SDType resType;
            if      (t == typeof(bool))   resType = SDType.Bool;
            else if (t == typeof(sbyte))  resType = SDType.SByte;
            else if (t == typeof(byte))   resType = SDType.Byte;
            else if (t == typeof(short))  resType = SDType.Short;
            else if (t == typeof(ushort)) resType = SDType.UShort;
            else if (t == typeof(int))    resType = SDType.Int;
            else if (t == typeof(uint))   resType = SDType.UInt;
            else if (t == typeof(long))   resType = SDType.Long;
            else if (t == typeof(ulong))  resType = SDType.ULong;
            else if (t == typeof(float))  resType = SDType.Float;
            else if (t == typeof(double)) resType = SDType.Double;
            else throw new NotSupportedException($"Unsupported type: {t}");
            return resType;
        }
        protected static int ItemSizeCastBySDtype(SDType sdtype)
        {
            switch (sdtype)
            {
                case SDType.Bool:   return sizeof(bool);
                case SDType.SByte:  return sizeof(sbyte);
                case SDType.Byte:   return sizeof(byte);
                case SDType.Short:  return sizeof(short);
                case SDType.UShort: return sizeof(ushort);
                case SDType.Int:    return sizeof(int);
                case SDType.UInt:   return sizeof(uint);
                case SDType.Long:   return sizeof(long);
                case SDType.ULong:  return sizeof(ulong);
                case SDType.Float:  return sizeof(float);
                case SDType.Double: return sizeof(double);
                default: throw new NotSupportedException($"Unsupported SDType: {sdtype}");
            }
        }
        protected static int ArrayNd(Array array)
        {
            return array.Rank;
        }
        protected static long[] ArrayDimensions(Array array, int nd)
        {
            long[] dimensions = new long[nd];
            for (int i = 0; i < nd; i++) {
                dimensions[i] = array.GetLength(i);  //各次元のサイズを取得
            }
            return dimensions;
        }
        protected static SDType ArraySDtype(Array array)
        {
            return TypeToSDType(array.GetType().GetElementType());
        }
        
        /// <summary> array to ndarray </summary>
        protected static IntPtr ArrayToNdArray(Array src) //src 解放しないver.
        {
            int nd = src.Rank;
            long[] dimensions = new long[nd];
            for (int i = 0; i < nd; i++) {
                dimensions[i] = src.GetLength(i);
            }
            Type type = src.GetType().GetElementType();
            SDType sdType = TypeToSDType(type);
            int itemSize = ItemSizeCastBySDtype(sdType);
            IntPtr result = ndarray_create(nd, dimensions, itemSize, 'c');
            return result;
        }

        /// <summary> エラー </summary>
        protected static string GetErrorMessage() => Marshal.PtrToStringAnsi(get_error_message()) ?? "unknown error.";
    }

    /// <summary> Native Code for NdArray </summary>
    public abstract partial class CLanguageNative
    {
        private const string DLL_Name = "CLanguageNative";

        // ----------------------------------------------------------------
        // 生成系
        // ----------------------------------------------------------------
        /// <summary> NdArrayCreate </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr ndarray_create(int nd, long[] dimensions, int itemsize, char order);
        /// <summary> NdArrayCopy </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr ndarray_copy(IntPtr src);
        /// <summary> Zeros </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_zeros(long[] size, int size_nd, SDType sdtype);
        /// <summary> Ones </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_ones(long[] size, int size_nd, SDType sdtype);
        /// <summary> Full </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_full(long[] size, int size_nd, double value, SDType sdtype);
        /// <summary> Empty </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_empty(long[] size, int size_nd, SDType sdtype);
        /// <summary> Arange </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_arange(int start, int end, int step, SDType sdType, char order);
        /// <summary> DArange </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_d_arange(double start, double end, double step, SDType sdtype, char order);
        /// <summary> Indices </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_indices(long[] shape, int shape_nd, SDType sdtype);

        // ----------------------------------------------------------------
        // 形状変換系
        // ----------------------------------------------------------------
        /// <summary> Reshape </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_reshape(IntPtr src, long[] size, int size_nd);
        /// <summary> Resize </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_resize(IntPtr src, long[] size, int size_nd);
        /// <summary> Ravel（1次元に平坦化） </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_ravel(IntPtr src);
        /// <summary> Squeeze </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_squeeze(IntPtr src);
        /// <summary> Transpose </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_transpose(IntPtr src, long[] size);
        /// <summary> Pad </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_pad(IntPtr src, int pad_width, PadModeType mode, double value, SDType sdtype);
        /// <summary> Concatenate </summary>
        protected static extern IntPtr np_concatenate(IntPtr[] arrays, int array_count, int axis, SDType sdtype);
        /// <summary> Stack </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_stack(IntPtr[] arrays, int array_count, uint axis, SDType sdtype);
        /// <summary> VStack </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_vstack(IntPtr[] arrays, int array_count);
        /// <summary> HStack </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_hstack(IntPtr[] arrays, int array_count);

        // ----------------------------------------------------------------
        // 条件・論理演算系
        // ----------------------------------------------------------------
        /// <summary> Where </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_where(IntPtr condition, IntPtr x, IntPtr y, SDType sdtype);
        /// <summary> LogicalAnd </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_logical_and(IntPtr a, IntPtr b);
        /// <summary> LogicalOr </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_logical_or(IntPtr a, IntPtr b);
        /// <summary> LogicalNot </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_logical_not(IntPtr a);

        // ----------------------------------------------------------------
        // 集計系
        // ----------------------------------------------------------------
        /// <summary> Sum NdArray </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_sum_return_array(IntPtr src, SDType srcType, SDType resType, int axis = -1, bool keepdims = false);
        /// <summary> Sum Scalar </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern long np_sum_return_scalar(IntPtr src, SDType srcType);
        /// <summary> Mean </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern double np_mean(IntPtr src, SDType sdtype);
        /// <summary> Max </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern double np_max(IntPtr src, SDType sdtype);
        /// <summary> Min </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern double np_min(IntPtr src, SDType sdtype);

        // ----------------------------------------------------------------
        // 演算系
        // ----------------------------------------------------------------
        /// <summary> Add </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_add(IntPtr a, IntPtr b, SDType sdtype);
        /// <summary> Subtract </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_subtract(IntPtr a, IntPtr b, SDType sdtype);
        /// <summary> Multiply </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_multiply(IntPtr a, IntPtr b, SDType sdtype);
        /// <summary> Divide </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_divide(IntPtr a, IntPtr b, SDType sdtype);

        // ----------------------------------------------------------------
        // ランダム系
        // ----------------------------------------------------------------
        /// <summary> RandomChoice </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_choice(IntPtr src, long[] size, int size_length, bool replace, float[] p, int p_length, SDType sdtype);
        /// <summary> RandomRand（0〜1の乱数） </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_rand(long[] size, int size_nd);
        /// <summary> RandomRandint </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_randint(int low, int high, long[] size, int size_nd);

        // ----------------------------------------------------------------
        // 型変換系
        // ----------------------------------------------------------------
        /// <summary> Cast（astype） </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_ndarray_cast(IntPtr src, SDType restype);
        
        // ----------------------------------------------------------------
        // プロパティ取得系
        // ----------------------------------------------------------------
        /// <summary> nd </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern int ndarray_nd(IntPtr src);
        
        /// <summary> size </summary>
        /// <summary> shape </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_shape(IntPtr src);

        /// <summary> dtype </summary>
        /// <summary> itemsize </summary>
        /// <summary> nbytes 総バイト数 </summary>
        /// <summary> strides 各次元のストライド </summary>
        /// <summary> data データバッファへのポインタ → これはなし </summary>
        
        /// <summary> ndarray_free </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern void ndarray_free(IntPtr src);

        /// <summary> GetErrorMessage </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr get_error_message();
    }
}