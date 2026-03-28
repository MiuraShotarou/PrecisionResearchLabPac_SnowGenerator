// Assets/Editor/FloatFieldDrawer.cs
#if UNITY_EDITOR
using System;
using UnityEditor;
using UnityEngine;

[CustomPropertyDrawer(typeof(FloatFieldAttribute))]
public class FloatFieldDrawer : PropertyDrawer
{
    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        var attr = (FloatFieldAttribute)attribute; //attributeがインスペクターの値を大量に格納している
        if (!string.IsNullOrEmpty(attr.name))
            label.text = attr.name;
        EditorGUI.BeginProperty(position, label, property);
        var fieldRect = EditorGUI.PrefixLabel(position, label);
        float buttonWidth = 20f;
        var minusRect = new Rect(fieldRect.x, fieldRect.y, buttonWidth, fieldRect.height);
        var inputRect = new Rect(fieldRect.x + buttonWidth, fieldRect.y, fieldRect.width - buttonWidth * 2, fieldRect.height);
        var plusRect  = new Rect(fieldRect.xMax - buttonWidth, fieldRect.y, buttonWidth, fieldRect.height);
        float value = (float)Math.Round(property.floatValue, attr.precision);
        if (GUI.Button(minusRect, "−"))
            value -= attr.step;
        value = EditorGUI.FloatField(inputRect, value);
        if (GUI.Button(plusRect, "+"))
            value += attr.step;
        value = (float)Math.Round(Mathf.Max(value, attr.min), attr.precision);
        property.floatValue = value;
        EditorGUI.EndProperty();
    }
}
#endif