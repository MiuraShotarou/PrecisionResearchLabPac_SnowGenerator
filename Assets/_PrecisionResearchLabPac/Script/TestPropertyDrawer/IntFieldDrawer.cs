#if UNITY_EDITOR
using UnityEditor;
using UnityEngine;

[CustomPropertyDrawer(typeof(IntFieldAttribute))]
public class IntFieldDrawer : PropertyDrawer
{
    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        var attr = (IntFieldAttribute)attribute;
        if (!string.IsNullOrEmpty(attr.name))
            label.text = attr.name;
        EditorGUI.BeginProperty(position, label, property);
        var fieldRect = EditorGUI.PrefixLabel(position, label);
        float buttonWidth = 20f;
        var minusRect = new Rect(fieldRect.x, fieldRect.y, buttonWidth, fieldRect.height);
        var inputRect = new Rect(fieldRect.x + buttonWidth, fieldRect.y, fieldRect.width - buttonWidth * 2, fieldRect.height);
        var plusRect  = new Rect(fieldRect.xMax - buttonWidth, fieldRect.y, buttonWidth, fieldRect.height);
        int value = property.intValue;
        if (GUI.Button(minusRect, "−"))
            value -= 1;
        value = EditorGUI.IntField(inputRect, value);
        if (GUI.Button(plusRect, "+"))
            value += 1;
        value = Mathf.Max(value, attr.min);
        property.intValue = value;
        EditorGUI.EndProperty();
    }
}
#endif