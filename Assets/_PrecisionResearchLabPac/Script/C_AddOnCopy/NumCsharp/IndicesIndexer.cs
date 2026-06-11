using UnityEngine;
using System.Collections.Generic;

namespace SnowflakeNative
{
    public partial struct Indices
    {
        /// <summary> Gets the indices array for the specified dimension. </summary>
        public long[] this[int index] => _values[index];
    }
}
