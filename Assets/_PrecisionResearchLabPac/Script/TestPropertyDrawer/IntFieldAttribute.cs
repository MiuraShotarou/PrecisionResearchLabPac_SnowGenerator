using UnityEngine;

public class IntFieldAttribute : PropertyAttribute
{
    public string name;
    public int min;
    public int softMax;

    public IntFieldAttribute(string name, int min, int softMax)
    {
        this.name = name;
        this.min = min;
        this.softMax = softMax;
    }
}
