// NdArray.cs
using System;
using System.Linq;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Cysharp.Threading.Tasks;
using Unity.VisualScripting;

/// <summary> 基本仕様 </summary>
// 静的型付け言語として実装する(ジェネリクスが異なるNdArray<T>同士の処理は原則行わない)
// ユーザーが明示していない型変換は禁止
// 式の右辺に複数のコンテナ(NdArray<T>, C#標準配列など)がある場合、戻り値の形状は必ず右辺のコンテナの中で最大の形状と同じになる（ブロードキャスト）
// 配列において次元の要素数が0若しくは1の時、その次元同士の計算では下位の次元をコピーして演算する。次元の要素数が2以上で演算対象の要素数がその値よりも大きかった場合、コンパイルエラーが出る（ブロードキャスト）
// 配列において次元の要素数が0若しくは1の時、その次元を拡張すると中身の要素に下位の次元のコピーが代入される。要素数が2以上の場合、拡張しようとするとコンパイルエラーが出る（ブロードキャスト）
// ユーザーの指定無しに破壊的操作は行わない
// NdArray<T>クラスの生成関数はジェネリクスの指定を強制し、それ以外はNdArray.の記述で呼び出しが可能
// NdArray<T>の破壊的操作を伴うメソッドは(関数名)+(NdArray<T>引数)の書き方で呼び出し可能
// Tにはunmanaged型と、string型を指定可能
// NdArrayではない型からNdArrayに変換する際は、NdArrayクラスのコンストラクタを使用する。従って、AsArrayメソッドは廃止する
// 使用される基本プリミティブ型はint, floatだが、大型開発用にdouble, longを基本に利用したメソッド群クラスを提供する
// NdArray.IsReferenceの場合、書き込み操作を禁止する

/// <summary> C言語側へ渡すSDTypeを削除すること </summary>
/// <summary> long, doubleなどのプリミティブ型操作見直し </summary>
/// <summary> NdArrayViewクラスを実装する </summary>
/// <summary> 実装しきれていないユーザーメソッドを実装する </summary>
/// <summary> _pointer のnullチェックを忘れずに記述すること </summary>
/// <summary> 現状、where T : unmanagedの付与により、Tへ参照型（stringなど）を渡すことができない </summary>
/// <summary> NdArrayを返すメソッドは破壊的操作なのか新規作成操作なのかを考慮して設計すること </summary>
/// <summary> NdArrayを引数に渡すメソッドは、かならずヒープメモリからの解放処理をC言語側に記述すること </summary>
/// <summary> C言語側で余分な大きさを持った配列を新規生成する場合は、不定値が入ってしまうことを念頭においてコーディングすること </summary>
/// <summary> overflow対策は一旦しなくて良い </summary>
/// <summary> dimensionsのlongはintにしたい </summary>
/// <summary> C言語側 np → ns にリネームしたい </summary>
/// <summary> C言語側 viewの転置処理 </summary>
/// <summary> C言語側 flags NDARRAY_FLAG_OWNDATA の追記 </summary>
/// <summary> C言語側 view によって操作を切り替える処理を追記すること </summary>
/// <summary> C言語側 static付与の修正 </summary>
/// <summary> C言語側 errorをgo to形式で記述。fail 内でのローカルポインタの解放 </summary>
/// <summary> C言語側 size_tの使用を検討 </summary>
/// <summary> C言語側 random.cの完成 </summary>
/// <summary> NdArrayのコンストラクタに、ユーザーが要素を指定して初期化できる実装を追加する </summary>
/// <summary> IDisposableインターフェイスの実装を検討 </summary>
/// <summary> CSLanguageNativeクラス内でジェネリクスを使用しない書き方にリファクタしたい </summary>
/// <summary> だいぶ先の話だが、Add関数やRemove関数も追加したいな </summary>
/// <summary> np_whereのindicesの戻り値を、出来ればNdArray<int>と<long>で自動切り替えができるように変更したいな </summary>
/// <summary> NdArrayError.csをコーディング </summary>
/// <summary> NdArrayRandomクラスは、seedをコンストラクタに受け取ることで再現性のある乱数配列を生成することができ、またシード値に指定がない場合でも完全なランダム乱数配列を生成できる仕様とする。アルゴリズムにはPCG64を使用する </summary>
/// <summary> RandomChoiceは関数名を揃えてオーバーロードで実装する </summary>
namespace SnowflakeNative
{
    /// <summary> Collection is NdArray </summary>
    public partial class NdArray<T> : CSLanguageNative, INdArray where T : unmanaged
    {
        private IntPtr _pointer; //DisPoseを実装すべき
        private bool _isReference;
        /// <summary> for client method </summary>
        public NdArray(long[] dimensions, char order = 'C')
        {
            int nd = dimensions.Length;
            int itemsize = Marshal.SizeOf(typeof(T));
            SDType sdType = GenericsToSDType<T>();
            _pointer = ndarray_create(nd, dimensions, itemsize, sdType);
            if (_pointer == IntPtr.Zero)
            {
                throw new InvalidOperationException("ndarray_create failed.");
            }
            _isReference = false;
        }
        public NdArray(T scalar, char order = 'C') //OK
        {
            // TODO
        }
        public NdArray(Array array, char order = 'C') //OK
        {
            // TODO
        }
        public NdArray(List<T> list, char order = 'C') //未実装
        {
            // TODO
        }
        /// <summary> INdArray </summary>
        IntPtr INdArray._pointer => this._pointer; //Indexerでのみ使用中
        SDType INdArray._sdtype => GenericsToSDType<T>();
        bool   INdArray.IsReference => this._isReference;

        /// <summary> client method </summary>
        public void Dispose() => CSDispose(this._pointer); //ジェネリクス指定をしたくない
        public bool IsView() => this._isReference;

        /// <summary> for developer method </summary>
        private NdArray()
        {
            _pointer = IntPtr.Zero;
        }
        private static NdArray<TResult> Packing<TResult>(NdArray<TResult> src, IntPtr pointer) where TResult : unmanaged
        {
            src._pointer = pointer;
            return src;
        }

        /// <summary> instance method </summary> //thisのTとメソッド内のTは同一の型として解釈される. //戻り値のTが引数のTと異なる可能性があるメソッドはTRsultを明記している。
        /// <summary> NdArrayCopy </summary>
        public NdArray<T> NdArrayCopy() => Packing(new NdArray<T>(), CSCopy(this._pointer));
        /// <summary> RandomChoice </summary>
        public NdArray<T> RandomChoice(long[] size, bool replace = true, float[] p = null) => Packing(new NdArray<T>(), CSRandomChoice(this._pointer, size, replace, p));
        /// <summary> Ravel </summary>
        public NdArray<T> Ravel() => Packing(new NdArray<T>(), CSRavel(this._pointer));
        /// <summary> Reshape </summary>
        public NdArray<T> Reshape(long[] size) => Packing(new NdArray<T>(), CSReshape(this._pointer, size));
        /// <summary> Sum </summary>
        public NdArray<T> Sum(int axis = -1, bool keepdims = false) => Packing(new NdArray<T>(), CSSum(this._pointer, axis, keepdims));
        public T Sum() => CSSum<T>(this._pointer);
        /// <summary> Cast </summary>
        public NdArray<TResult> Cast<TResult>() where TResult : unmanaged => Packing(new NdArray<TResult>(), CSArrayCast<TResult>(this._pointer));
        /// <summary> Pad </summary>
        public NdArray<T> Pad(int pad_width, PadModeType mode, double value) => Packing(new NdArray<T>(), CSPad(this._pointer, pad_width, mode, value));
        /// <summary> BroadcastTo </summary>
        public NdArray<T> BroadcastTo(long[] size) => Packing(new NdArray<T>(), CSBroadcastTo(this._pointer, size));
        
        /// <summary> no instance method compulsion T </summary> //NdArray<T>.(メソッド名)でメソッド内のTを強制指定
        /// <summary> Zeros </summary>
        public static NdArray<T> Zeros(long[] size) => Packing(new NdArray<T>(), CSZeros<T>(size));
        /// <summary> Ones </summary>
        public static NdArray<T> Ones(long[] size) => Packing(new NdArray<T>(), CSOnes<T>(size));
        /// <summary> Full </summary>
        public static NdArray<T> Full(long[] size, double value) => Packing(new NdArray<T>(), CSFull<T>(size, value));
        
        /// <summary> no instance method not compulsion T </summary> //NdArray<T>.(メソッド名)でメソッド内のTを指定。NdArray.(メソッド名)で指定しないことも可能
        /// <summary> Arange </summary>
        public static NdArray<T> Arange(int end, char order) => Packing(new NdArray<T>(), CSArange<T>(0, end, 1, order));
        public static NdArray<T> Arange(int start, int end, char order) => Packing(new NdArray<T>(), CSArange<T>(start, end, 1, order));
        public static NdArray<T> Arange(int start, int end, int step, char order) => Packing(new NdArray<T>(), CSArange<T>(start, end, step, order));
        /// <summary> LArange </summary>
        public static NdArray<T> LArange(double end, char order) => Packing(new NdArray<T>(), CSDArange<T>(0, end, 1, order));
        public static NdArray<T> LArange(double start, double end, char order) => Packing(new NdArray<T>(), CSDArange<T>(start, end, 1, order));
        public static NdArray<T> LArange(double start, double end, double step, char order) => Packing(new NdArray<T>(), CSDArange<T>(start, end, step, order));
        /// <summary> Where </summary>
        public static NdArray<long> Where(NdArray<bool> conditions) => Packing(new NdArray<long>(), CSWhere(conditions._pointer));
        public static NdArray<T> Where(NdArray<bool> conditions, NdArray<T> a, NdArray<T> b) => Packing(new NdArray<T>(), CSWhere(conditions._pointer, a._pointer, b._pointer));
        /// <summary> Stack </summary>
        public static NdArray<T> Stack(NdArray<T>[] srcArray, uint axis = 0) => Packing(new NdArray<T>(), CSStack<T>(srcArray.Select(arr => arr._pointer).ToArray(), axis));
        public static NdArray<T> VStack(NdArray<T>[] srcArray) => Packing(new NdArray<T>(), CSVStack(srcArray.Select(arr => arr._pointer).ToArray()));

        /// <summary> Concatenate </summary>
        public static NdArray<T> Concatenate(IntPtr[] srcArray, int axis = 0) => Packing(new NdArray<T>(), CSConcatenate<T>(srcArray, axis));
    }

    public static class NdArray
    {
        public static NdArray<T> Where<T>(NdArray<bool> conditions, NdArray<T> a, NdArray<T> b) where T : unmanaged => NdArray<T>.Where(conditions, a, b);
        public static NdArray<int> Arange(int start, int end, int step, char order) => NdArray<int>.Arange(start, end, step, order);
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
        
        protected static IntPtr CSZeros<T>(long[] size) where T : unmanaged
        {
            int size_nd = size.Length;
            SDType sdtype = GenericsToSDType<T>();
            return np_zeros(size, size_nd, sdtype);
        }
        protected static IntPtr CSOnes<T>(long[] size) where T : unmanaged
        {
            int size_nd = size.Length;
            SDType sdtype = GenericsToSDType<T>();
            return np_ones(size, size_nd, sdtype);
        }
        protected static IntPtr CSFull<T>(long[] size, double value) where T : unmanaged
        {
            SDType sdtype = GenericsToSDType<T>();
            int size_nd = size.Length;
            IntPtr result = np_full(size, size_nd, value, sdtype);
            if (result == IntPtr.Zero) {
                throw new InvalidOperationException(GetErrorMessage());
            }
            return result;
        }
        protected static IntPtr CSArange<T>(int start, int end, int step, char order) where T : unmanaged
        {
            SDType resType = GenericsToSDType<T>();
            return np_arange(start, end, step, resType, order); //orderはC言語で処理するかFortranで処するかを指定する
        }
        protected static IntPtr CSDArange<T>(double start, double end, double step, char order) where T : unmanaged
        {
            SDType resType = GenericsToSDType<T>();
            return np_d_arange(start, end, step, resType, order);
        }
        protected static IntPtr CSArrayCast<TResult>(IntPtr pointer) where TResult : unmanaged
        {
            SDType resType = GenericsToSDType<TResult>();
            // pointerをC言語側へ渡す
            IntPtr result = np_ndarray_cast(pointer, resType); //SDTypeでどのメソッドを呼び出すのか決めている。+ Source元の配列のポインタから具体的な型のついたポインタに変換する必要がある。それを、C言語側で行う。C#側のNdArrayは用意しなくて良い。
            if (result == IntPtr.Zero) {
                throw new InvalidOperationException(GetErrorMessage());
            }
            return result;
        }
        protected static IntPtr CSRandomChoice_Scalar<T>(int max, int count, bool replace = true) where T : unmanaged //値の重複有り
        {
            SDType sdType = GenericsToSDType<T>();
            return np_random_choice_argumentscalar(max, count, replace, sdType);
        }
        protected static IntPtr CSRandomChoice_NdArray(IntPtr values, int count, bool replace = true)
        {
            return np_random_choice_argumentndarray(values, count, replace);
        }
        /// <summary>  </summary>
        protected static IntPtr CSPad(IntPtr pointer, int pad_wdith, PadModeType mode = PadModeType.Constant, double value = 0)
        {
            return np_pad(pointer, pad_wdith, mode, value);
        }

        protected static IntPtr CSConcatenate<T>(IntPtr[] pointerArray, int axis = 0) where T : unmanaged
        {
            if (pointerArray.Any(ptr => ptr == IntPtr.Zero)) {
                throw new ArgumentException(GetErrorMessage());
            }
            int array_count = pointerArray.Length;
            SDType resType = GenericsToSDType<T>();
            return np_concatenate(pointerArray, array_count, axis, resType);
        }
        /// <summary> 配列Aに新しい次元として配列Bを追加し、新規配列として返す </summary>
        protected static IntPtr CSStack<T>(IntPtr[] pointerArray, uint axis = 0) where T : unmanaged
        {
            SDType resType = GenericsToSDType<T>();
            int pointerCount = pointerArray.Length;
            return np_stack(pointerArray, pointerCount, axis, resType); //形状チェックはC言語側で行う
        }
        /// <summary> 配列を縦方向に順番に積み重ねる </summary>
        protected static IntPtr CSVStack(IntPtr[] pointerArray)
        {
            int array_count = pointerArray.Length;
            return np_vstack(pointerArray, array_count);
        }
        protected static IntPtr CSHstack(IntPtr[] pointerArray)
        {
            int array_count = pointerArray.Length;
            return np_hstack(pointerArray, array_count);
        }

        /// <summary> np_broadcast_to </summary>
        protected static IntPtr CSBroadcastTo(IntPtr pointer, long[] size)
        {
            int size_nd = size.Length;
            return np_broadcast_to(pointer, size, size_nd);
        }
        
        protected static IntPtr CSSqueeze<T>(IntPtr pointer) where T : unmanaged
        {
           return np_squeeze(pointer);
        }
        
        /// <summary> 配列での受け取り </summary>
        protected static IntPtr CSSum(IntPtr pointer, int axis = -1, bool keepdims = false)
        {
            return np_sum_return_array(pointer, axis, keepdims);
        }
        /// <summary> スカラーでの受け取り </summary>
        protected static T CSSum<T>(IntPtr pointer) where T : unmanaged => (T)Convert.ChangeType(np_sum_return_scalar(pointer), typeof(T));
        
        protected static IntPtr CSRavel(IntPtr pointer)
        {
            return np_ravel(pointer);
        }
        protected static IntPtr CSReshape(IntPtr pointer, long[] size)
        {
            int size_nd = size.Length;
            return np_reshape(pointer, size, size_nd);
        }
        /// <summary> Resize </summary>
        protected static IntPtr CSResize(IntPtr pointer, long[] size)
        {
            int size_nd = size.Length;
            IntPtr result = np_resize(pointer, size, size_nd);
            // if (result == IntPtr.Zero) {
            //     throw new InvalidOperationException(GetErrorMessage());
            // }
            return result;
        }
        /// <summary> Where </summary>
        protected static IntPtr CSWhere(IntPtr conditions, IntPtr trueValue, IntPtr falseValue) //return x or y
        {
            // conditions.sdtype == bool
            IntPtr result = np_where(conditions, trueValue, falseValue);
            // if (result == IntPtr.Zero) {
            //     throw new InvalidOperationException(GetErrorMessage());
            // }
            return result;
        }
        protected static IntPtr CSWhere(IntPtr conditions) //return long_ndarray
        {
            // conditions.sdtype == bool
            IntPtr result = np_where(conditions, IntPtr.Zero, IntPtr.Zero);
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
        /// <summary> Scalar To NdArray </summary>
        protected static IntPtr CSScalarToNdArray<T>(T value) where T : unmanaged
        {
            // value == structを不可に、stringを有りにする。valueに参照型を渡すことはできない
            SDType sdType = GenericsToSDType<T>();
            int itemsize = ItemSizeCastBySDtype(sdType);
            // スタック上にメモリを確保してscalarをコピー
            IntPtr pointer = Marshal.AllocHGlobal(itemsize);
            Marshal.StructureToPtr(value, pointer, false);
            // scalarをNdArrayに変換する処理
            int nd = 1;
            long[] dimensions = new long[nd];
            return ndarray_convert(pointer, nd, dimensions, itemsize, sdType);
        }
        /// <summary> Array To NdArray </summary>
        protected static IntPtr ArrayToNdArray(Array src) //src 解放しないver.
        {
            int nd = ArrayNd(src);
            long[] dimensions = ArrayDimensions(src, nd);
            SDType sdType = ArraySDtype(src);
            int itemSize = ItemSizeCastBySDtype(sdType);
            GCHandle handle = GCHandle.Alloc(src, GCHandleType.Pinned); //
            IntPtr srcPtr = handle.AddrOfPinnedObject();
            IntPtr result = ndarray_convert(srcPtr, nd, dimensions, itemSize, sdType);
            
            handle.Free();
            
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
        protected static extern IntPtr ndarray_create(int nd, long[] dimensions, int itemsize, SDType sdType);
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
        /// <summary> LArange </summary>
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
        protected static extern IntPtr np_pad(IntPtr src, int pad_width, PadModeType mode, double value);
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
        // ブロードキャスト系
        // ----------------------------------------------------------------
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_broadcast_to(IntPtr array, long[] size, int size_nd);

        // ----------------------------------------------------------------
        // 集計系
        // ----------------------------------------------------------------
        /// <summary> Sum NdArray </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_sum_return_array(IntPtr src, int axis = -1, bool keepdims = false);
        /// <summary> Sum Scalar </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern double np_sum_return_scalar(IntPtr src);
        /// <summary> Mean </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern double np_mean(IntPtr src);
        /// <summary> Max </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern double np_max(IntPtr src);
        /// <summary> Min </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern double np_min(IntPtr src);
        
        // ----------------------------------------------------------------
        // Where
        // ----------------------------------------------------------------
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_where(IntPtr condition, IntPtr x, IntPtr y);

        // ----------------------------------------------------------------
        // ランダム系
        // ----------------------------------------------------------------
        /// <summary> RandomChoice </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_choice_argumentscalar(int max, int count, bool replace, SDType sdtype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_random_choice_argumentndarray(IntPtr values, int count, bool replace);        /// <summary> RandomChoice </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_d_random_choice_argumentscalar(double max, double count, bool replace, SDType sdtype);
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_d_random_choice_argumentndarray(IntPtr values, double count, bool replace);
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
        /// <summary> shape </summary>
        [DllImport(DLL_Name, CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr np_shape(IntPtr src);
        
        /// <summary> size </summary>

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