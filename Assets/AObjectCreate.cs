using UnityEngine;

[ExecuteAlways]
public class AObjectCreate : MonoBehaviour
{
    public void GenerateCube()
    {
        GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
        cube.transform.position = new Vector3(0, 0, 0);
        cube.transform.rotation = Quaternion.identity;
        cube.transform.localScale = new Vector3(1, 1, 1);
        cube.name = "ACube";
    }
}