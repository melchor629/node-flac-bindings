{
    "targets": [{
        "target_name": "flac-bindings",
        "sources": [
            "src/index.cpp",
            "src/encoder.cpp",
            "src/decoder.cpp",
            "src/format.cpp",
            "src/metadata0.cpp",
            "src/metadata1.cpp",
            "src/metadata2.cpp",
            "src/metadata.cpp",
            "src/mappings/frame.cpp",
            "src/mappings/streammetadata.cpp",
            "src/mappings/streammetadata_application.cpp",
            "src/mappings/streammetadata_cuesheet.cpp",
            "src/mappings/streammetadata_padding.cpp",
            "src/mappings/streammetadata_picture.cpp",
            "src/mappings/streammetadata_seektable.cpp",
            "src/mappings/streammetadata_streaminfo.cpp",
            "src/mappings/streammetadata_unknown.cpp",
            "src/mappings/streammetadata_vorbiscomment.cpp",
            "src/mappings/streammetadata_seekpoint.cpp",
            "src/mappings/streammetadata_cuesheet_track.cpp",
            "src/mappings/streammetadata_cuesheet_index.cpp"
        ],
        "include_dirs": [
            "<!(node -e \"require('nan')\")"
        ],
        "conditions": [
            [ 'OS=="mac"', {
                "xcode_settings": {
                    "MACOSX_DEPLOYMENT_TARGET": "10.9"
                },
            }]
        ]
    }
]}
