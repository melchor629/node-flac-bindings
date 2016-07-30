{
    "targets": [{
        "target_name": "flac-bindings",
        "sources": [
            "src/index.cpp",
            "src/encoder.cpp",
            "src/decoder.cpp",
            "src/format.cpp",
            "src/metadata0.cpp",
            "src/metadata.cpp"
        ],
        "include_dirs": [
            "<!(node -e \"require('nan')\")"
        ],
        "conditions": [
            [ 'OS=="mac"', {
                "xcode_settings": {
                    "MACOSX_DEPLOYMENT_TARGET": "10.7"
                },
            }]
        ]
    }
]}
