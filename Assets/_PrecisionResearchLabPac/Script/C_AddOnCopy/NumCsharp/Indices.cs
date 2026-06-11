using UnityEngine;
using System.Collections.Generic;

namespace SnowflakeNative
{
    public partial struct Indices
    {
        private List<long[]> _values;
        public Indices(List<long[]> values)
        {
            _values = values;
        }
    }
}
