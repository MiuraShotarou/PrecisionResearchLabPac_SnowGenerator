using UnityEngine;

/// <summary> GUIアトリビュートの作成 </summary>
public class FloatFieldAttribute : PropertyAttribute
{
    public string name;
    public float min;
    public float softMax;
    public int precision;
    public float step;

    public FloatFieldAttribute(string name, float min, float softMax, int precision, float step)
    {
        this.name = name;
        this.min = min;
        this.softMax = softMax;
        this.precision = precision;
        this.step = step;
    }
    public FloatFieldAttribute(string name, float min, float softMax)
    {
        this.name = name;
        this.min = min;
        this.softMax = softMax;
    }
    public FloatFieldAttribute(string name, float min)
    {
        this.name = name;
        this.min = min;
    }
    public FloatFieldAttribute(string name)
    {
        this.name = name;
    }
}