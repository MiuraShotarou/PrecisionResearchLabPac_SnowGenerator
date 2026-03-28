using System;
using UnityEngine;
using System.Collections.Generic;
using UnityEditor;
[ExecuteAlways]
public class ObjectProperties : MonoBehaviour //MonoBehaviourは正しい
{
    public Dictionary<string, object> __annotations__ => annotations;
    private Dictionary<string, object> annotations = new Dictionary<string, object>();
    //C#
    public Operators.SnowflakeGrowModal SnowflakeGrowModal;
    private void OnEnable(){
        EditorApplication.update += OnUpdate;
        }
    private void OnUpdate()
    {
        if (SnowflakeGrowModal != null) {
            if (growing)
            {
                SnowflakeGrowModal.Modal(this);
            }}
    }
    private void OnDisable(){
        EditorApplication.update -= OnUpdate;
    }

    private void OnValidate()
    {
        annotations = new Dictionary<string, object>
        {
            { "rho",              rho              },
            { "beta",             beta             },
            { "alpha",            alpha            },
            { "theta",            theta            },
            { "kappa",            kappa            },
            { "mu",               mu               },
            { "gamma",            gamma            },
            { "sigma",            sigma            },
            { "deltaRho",         deltaRho         },
            { "exportAnimation",  exportAnimation  },
            { "animationFilepath",animationFilepath},
            { "applyAnimation",   applyAnimation   },
            { "stepsPerFrame",    stepsPerFrame    },
            { "growing",          growing          },
            { "steps",            steps            }
        };
    }

    public Texture2D data;

    [Tooltip("Vapor density")]
    [FloatField("ρ", min: 0f, softMax: 1f, precision: 2, step: 0.1f)]
    public float rho = new SnowflakeParams().rho;

    [Tooltip("Attachement anisotropy")]
    [FloatField("β", min: 0f, softMax: 3f, precision: 2, step: 0.1f)]
    public float beta = new SnowflakeParams().beta;

    [Tooltip("Knife-edge instability")]
    [FloatField("α", min: 0f, softMax: 0.5f, precision: 3, step: 0.01f)]
    public float alpha = new SnowflakeParams().alpha;

    [Tooltip("Knife-edge instability")]
    [FloatField("θ", min: 0f, softMax: 0.2f, precision: 4, step: 0.01f)]
    public float theta = new SnowflakeParams().theta;

    [Tooltip("Crystallization/freezing strength")]
    [FloatField("κ", min: 0f, softMax: 0.2f, precision: 4, step: 0.01f)]
    public float kappa = new SnowflakeParams().kappa;

    [Tooltip("Boundary mass melting strength")]
    [FloatField("μ", min: 0f, softMax: 0.2f, precision: 3, step: 0.01f)]
    public float mu = new SnowflakeParams().mu;

    [Tooltip("Crystall mass melting strength")]
    [FloatField("γ", min: 0f, softMax: 0.01f, precision: 6, step: 0.0001f)]
    public float gamma = new SnowflakeParams().gamma;

    [Tooltip("Randomness")]
    [FloatField("σ", min: 0f, softMax: 1f, precision: 3, step: 0.1f)]
    public float sigma = new SnowflakeParams().sigma;

    [Tooltip("Relative vapor density update in each step")]
    [FloatField("Δρ", min: -1f, softMax: 0.0001f, precision: 5, step: 0.00001f)]
    public float deltaRho = 0.0f;

    [Tooltip("Export the growth process to an animated USD file")]
    public bool exportAnimation = false;

    [Tooltip("Path of the USD file to save the animation")]
    public string animationFilepath = "snowflake.usd";

    [Tooltip("Apply the exported animation to the snowflake using a mesh sequence cache modifier")]
    public bool applyAnimation = true;

    [Tooltip("Number of simulation steps per animation frame; speed of animation")]
    [IntField("Steps per frame", 1, 100)]
    public int stepsPerFrame = 1;

    public bool growing = false;

    [IntField("Simulation steps", 0, 1000)]
    public int steps = 0;

    // randomフラグ
    public bool randomRho      = false;
    public bool randomBeta     = false;
    public bool randomAlpha    = false;
    public bool randomTheta    = false;
    public bool randomKappa    = false;
    public bool randomMu       = false;
    public bool randomGamma    = false;
    public bool randomSigma    = false;
    public bool randomDeltaRho = false;

    [Tooltip("Thickness of the snowflake")]
    [FloatField("Thickness", min: 0f, softMax: 1f, precision: 2, step: 0.01f)]
    public float thickness = 0.01f;

    [Tooltip("Overall scale of the snowflake")]
    [FloatField("Scale", min: 0f, softMax: 1f, precision: 2, step: 0.01f)]
    public float scale = 0.01f;
}