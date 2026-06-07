// SDType 指定で下記の(配列同士キャスト関数)を呼び出すことができるメソッド → 破壊的操作で変更する形に → 内部でsrcarray_to_resarray_tableを呼び出す
NdArray *np_cast (NdArray *src, SDType restype); //NdArray.dataに反映させれば良い → np_cast にリネーム