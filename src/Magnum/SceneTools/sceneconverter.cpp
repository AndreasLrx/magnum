/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once Arguments is std::string-free */
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/SceneTools/FlattenMeshHierarchy.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Trade/AbstractSceneConverter.h"

#include "Magnum/Implementation/converterUtilities.h"
#include "Magnum/SceneTools/Implementation/sceneConverterUtilities.h"

namespace Magnum {

/** @page magnum-sceneconverter Scene conversion utility
@brief Converts scenes of different formats
@m_since{2020,06}

@tableofcontents
@m_footernavigation
@m_keywords{magnum-sceneconverter sceneconverter}

This utility is built if `MAGNUM_WITH_SCENECONVERTER` is enabled when building
Magnum. To use this utility with CMake, you need to request the
`sceneconverter` component of the `Magnum` package and use the
`Magnum::sceneconverter` target for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED imageconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::sceneconverter ...)
@endcode

See @ref building and @ref cmake and the @ref Trade namespace for more
information. There's also a corresponding @ref magnum-imageconverter "image conversion utility".

@section magnum-sceneconverter-example Example usage

Listing contents of a glTF file, implicitly using
@relativeref{Trade,AnySceneImporter} that delegates to
@relativeref{Trade,GltfImporter}, @relativeref{Trade,AssimpImporter} or
@ref file-formats "any other plugin capable of glTF import" depending on what's
available:

@m_class{m-code-figure}

@parblock

@code{.sh}
magnum-sceneconverter --info Box.gltf
@endcode

<b></b>

@m_class{m-nopad}

@include sceneconverter-info.ansi

@endparblock

Converting an OBJ file to a PLY, implicitly using
@relativeref{Trade,AnySceneConverter} that delegates to
@relativeref{Trade,StanfordSceneConverter} or
@ref file-formats "any other plugin capable of PLY export" depending on what's
available:

@code{.sh}
magnum-sceneconverter chair.obj chair.ply
@endcode

Processing an OBJ file with @relativeref{Trade,MeshOptimizerSceneConverter},
setting @ref Trade-MeshOptimizerSceneConverter-configuration "plugin-specific configuration options"
to reduce the index count to half, saving as a PLY, with verbose output showing
the processing stats:

@code{.sh}
magnum-sceneconverter chair.obj -C MeshOptimizerSceneConverter \
    -c simplify=true,simplifyTargetIndexCountThreshold=0.5 chair.ply -v
@endcode

@section magnum-sceneconverter-usage Full usage documentation

@code{.sh}
magnum-sceneconverter [-h|--help] [-I|--importer PLUGIN]
    [-C|--converter PLUGIN]... [--plugin-dir DIR] [--map]
    [--only-attributes N1,N2-N3…] [--remove-duplicates]
    [--remove-duplicates-fuzzy EPSILON]
    [-i|--importer-options key=val,key2=val2,…]
    [-c|--converter-options key=val,key2=val2,…]... [--mesh MESH]
    [--level LEVEL] [--concatenate-meshes] [--info-animations] [--info-images]
    [--info-lights] [--info-cameras] [--info-materials] [--info-meshes]
    [--info-objects] [--info-scenes] [--info-skins] [--info-textures] [--info]
    [--color on|4bit|off|auto] [--bounds] [-v|--verbose] [--profile] [--] input
    output
@endcode

Arguments:

-   `input` --- input file
-   `output` --- output file; ignored if `--info` is present
-   `-h`, `--help` --- display this help message and exit
-   `-I`, `--importer PLUGIN` --- scene importer plugin (default:
    @ref Trade::AnySceneImporter "AnySceneImporter")
-   `-C`, `--converter PLUGIN` --- scene converter plugin(s)
-   `--plugin-dir DIR` --- override base plugin dir
-   `--map` --- memory-map the input for zero-copy import (works only for
    standalone files)
-   `--only-attributes N1,N2-N3…` --- include only attributes of given IDs in
    the output. See @ref Utility::String::parseNumberSequence() for syntax
    description.
-   `--remove-duplicates` --- remove duplicate vertices using
    @ref MeshTools::removeDuplicates(const Trade::MeshData&) after import
-   `--remove-duplicates-fuzzy EPSILON` --- remove duplicate vertices using
    @ref MeshTools::removeDuplicatesFuzzy(const Trade::MeshData&, Float, Double)
    after import
-   `-i`, `--importer-options key=val,key2=val2,…` --- configuration options to
    pass to the importer
-   `-c`, `--converter-options key=val,key2=val2,…` --- configuration options
    to pass to the converter(s)
-   `--mesh MESH` --- mesh to import (default: `0`), ignored if
    `--concatenate-meshes` is specified
-   `--level LEVEL` --- mesh level to import (default: `0`), ignored if
    `--concatenate-meshes` is specified
-   `--concatenate-meshes` -- flatten mesh hierarchy and concatenate them all
    together @m_class{m-label m-warning} **experimental**
-   `--info-animations` --- print into about animations in the input file and
    exit
-   `--info-images` --- print into about images in the input file and exit
-   `--info-lights` --- print into about lights in the input file and exit
-   `--info-cameras` --- print into about cameras in the input file and exit
-   `--info-materials` --- print into about materials in the input file and
    exit
-   `--info-meshes` --- print into about meshes in the input file and exit
-   `--info-objects` --- print into about objects in the input file and exit
-   `--info-scenes` --- print into about scenes in the input file and exit
-   `--info-skins` --- print into about skins in the input file and exit
-   `--info-textures` --- print into about textures in the input file and exit
-   `--info` --- print info about everything in the input file and exit, same
    as specifying all other `--info-*` options together
-   `--color` --- colored output for `--info` (default: `auto`)
-   `--bounds` --- show bounds of known attributes in `--info` output
-   `-v`, `--verbose` --- verbose output from importer and converter plugins
-   `--profile` --- measure import and conversion time

If any of the `--info-*` options are given, the utility will print information
about given data present in the file. In this case no conversion is done and
output file doesn't need to be specified. In case one data references another
and both `--info-*` options are specified, the output will also list reference
count (for example, `--info-scenes` together with `--info-meshes` will print
how many objects reference given mesh).

The `-i` / `--importer-options` and `-c` / `--converter-options` arguments
accept a comma-separated list of key/value pairs to set in the importer /
converter plugin configuration. If the `=` character is omitted, it's
equivalent to saying `key=true`; configuration subgroups are delimited with
`/`.

It's possible to specify the `-C` / `--converter` option (and correspondingly
also `-c` / `--converter-options`) multiple times in order to chain more
converters together. All converters in the chain have to support the
@ref Trade::SceneConverterFeature::ConvertMesh feature,
the last converter either @ref Trade::SceneConverterFeature::ConvertMesh or
@ref Trade::SceneConverterFeature::ConvertMeshToFile. If the last converter
doesn't support conversion to a file,
@ref Trade::AnySceneConverter "AnySceneConverter" is used to save its output;
if no `-C` / `--converter` is specified,
@ref Trade::AnySceneConverter "AnySceneConverter" is used.

If `--concatenate-meshes` is given, all meshes of the input file are
concatenated into a single mesh using @ref MeshTools::concatenate(), with the
scene hierarchy transformation baked in using
@ref SceneTools::flattenMeshHierarchy3D(). Only attributes that are present in
the first mesh are taken, if `--only-attributes` is specified as well, the IDs
reference attributes of the first mesh.
*/

}

using namespace Magnum;
using namespace Containers::Literals;

namespace {

bool isInfoRequested(const Utility::Arguments& args) {
    return args.isSet("info-animations") ||
           args.isSet("info-images") ||
           args.isSet("info-lights") ||
           args.isSet("info-cameras") ||
           args.isSet("info-materials") ||
           args.isSet("info-meshes") ||
           args.isSet("info-objects") ||
           args.isSet("info-scenes") ||
           args.isSet("info-skins") ||
           args.isSet("info-textures") ||
           args.isSet("info");
}

}

int main(int argc, char** argv) {
    Utility::Arguments args;
    args.addArgument("input").setHelp("input", "input file")
        .addArgument("output").setHelp("output", "output file; ignored if --info is present")
        .addOption('I', "importer", "AnySceneImporter").setHelp("importer", "scene importer plugin", "PLUGIN")
        .addArrayOption('C', "converter").setHelp("converter", "scene converter plugin(s)", "PLUGIN")
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
        .addBooleanOption("map").setHelp("map", "memory-map the input for zero-copy import (works only for standalone files)")
        #endif
        .addOption("only-attributes").setHelp("only-attributes", "include only attributes of given IDs in the output", "N1,N2-N3…")
        .addBooleanOption("remove-duplicates").setHelp("remove-duplicates", "remove duplicate vertices in the mesh after import")
        .addOption("remove-duplicates-fuzzy").setHelp("remove-duplicates-fuzzy", "remove duplicate vertices with fuzzy comparison in the mesh after import", "EPSILON")
        .addOption('i', "importer-options").setHelp("importer-options", "configuration options to pass to the importer", "key=val,key2=val2,…")
        .addArrayOption('c', "converter-options").setHelp("converter-options", "configuration options to pass to the converter(s)", "key=val,key2=val2,…")
        .addOption("mesh", "0").setHelp("mesh", "mesh to import, ignored if --concatenate-meshes is specified")
        .addOption("level", "0").setHelp("level", "mesh level to import, ignored if --concatenate-meshes is specified")
        .addBooleanOption("concatenate-meshes").setHelp("concatenate-meshes", "flatten mesh hierarchy and concatenate them all together")
        .addBooleanOption("info-animations").setHelp("info-animations", "print info about animations in the input file and exit")
        .addBooleanOption("info-images").setHelp("info-images", "print info about images in the input file and exit")
        .addBooleanOption("info-lights").setHelp("info-lights", "print info about images in the input file and exit")
        .addBooleanOption("info-cameras").setHelp("info-cameras", "print info about cameras in the input file and exit")
        .addBooleanOption("info-materials").setHelp("info-materials", "print info about materials in the input file and exit")
        .addBooleanOption("info-meshes").setHelp("info-meshes", "print info about meshes in the input file and exit")
        .addBooleanOption("info-objects").setHelp("info-objects", "print info about objects in the input file and exit")
        .addBooleanOption("info-scenes").setHelp("info-scenes", "print info about scenes in the input file and exit")
        .addBooleanOption("info-skins").setHelp("info-skins", "print info about skins in the input file and exit")
        .addBooleanOption("info-textures").setHelp("info-textures", "print info about textures in the input file and exit")
        .addBooleanOption("info").setHelp("info", "print info about everything in the input file and exit, same as specifying all other --info-* options together")
        .addOption("color", "auto").setHelp("color", "colored output for --info", "on|4bit|off|auto")
        .addBooleanOption("bounds").setHelp("bounds", "show bounds of known attributes in --info output")
        .addBooleanOption('v', "verbose").setHelp("verbose", "verbose output from importer and converter plugins")
        .addBooleanOption("profile").setHelp("profile", "measure import and conversion time")
        .setParseErrorCallback([](const Utility::Arguments& args, Utility::Arguments::ParseError error, const std::string& key) {
            /* If --info is passed, we don't need the output argument */
            if(error == Utility::Arguments::ParseError::MissingArgument &&
                key == "output" && isInfoRequested(args)) return true;

            /* Handle all other errors as usual */
            return false;
        })
        .setGlobalHelp(R"(Converts scenes of different formats.

If any of the --info-* options are given, the utility will print information
about given data present in the file. In this case no conversion is done and
output file doesn't need to be specified. In case one data references another
and both --info-* options are specified, the output will also list reference
count (for example, --info-scenes together with --info-meshes will print how
many objects reference given mesh).

The -i / --importer-options and -c / --converter-options arguments accept a
comma-separated list of key/value pairs to set in the importer / converter
plugin configuration. If the = character is omitted, it's equivalent to saying
key=true; configuration subgroups are delimited with /.

It's possible to specify the -C / --converter option (and correspondingly also
-c / --converter-options) multiple times in order to chain more converters
together. All converters in the chain have to support the ConvertMesh feature,
the last converter either ConvertMesh or ConvertMeshToFile. If the last
converter doesn't support conversion to a file, AnySceneConverter is used to
save its output; if no -C / --converter is specified, AnySceneConverter is
used.

If --concatenate-meshes is given, all meshes of the input file are concatenated
into a single mesh, with the scene hierarchy transformation baked in. Only
attributes that are present in the first mesh are taken, if --only-attributes
is specified as well, the IDs reference attributes of the first mesh.)")
        .parse(argc, argv);

    /* Colored output. Enable only if a TTY. */
    Debug::Flags useColor;
    bool useColor24;
    if(args.value("color") == "on") {
        useColor = Debug::Flags{};
        useColor24 = true;
    } else if(args.value("color") == "4bit") {
        useColor = Debug::Flags{};
        useColor24 = false;
    } else if(args.value("color") == "off") {
        useColor = Debug::Flag::DisableColors;
        useColor24 = false;
    } else if(Debug::isTty()) {
        useColor = Debug::Flags{};
        /* https://unix.stackexchange.com/a/450366, not perfect but good enough
           I'd say */
        /** @todo make this more robust and put directly on Debug,
            including a "Disable 24 colors" flag */
        const Containers::StringView colorterm = std::getenv("COLORTERM");
        useColor24 = colorterm == "truecolor"_s || colorterm == "24bit"_s;
    } else {
        useColor = Debug::Flag::DisableColors;
        useColor24 = false;
    }

    /* Generic checks */
    if(!args.value<Containers::StringView>("output").isEmpty()) {
        /* Not an error in this case, it should be possible to just append
           --info* to existing command line without having to remove anything.
           But print a warning at least, it could also be a mistyped option. */
        if(isInfoRequested(args))
            Warning{} << "Ignoring output file for --info:" << args.value<Containers::StringView>("output");
    }

    /* Importer manager */
    PluginManager::Manager<Trade::AbstractImporter> importerManager{
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::split(Trade::AbstractImporter::pluginSearchPaths().back()).second())};

    /* Scene converter manager */
    PluginManager::Manager<Trade::AbstractSceneConverter> converterManager{
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::split(Trade::AbstractSceneConverter::pluginSearchPaths().back()).second())};

    Containers::Pointer<Trade::AbstractImporter> importer = importerManager.loadAndInstantiate(args.value("importer"));
    if(!importer) {
        Debug{} << "Available importer plugins:" << ", "_s.join(importerManager.aliasList());
        return 1;
    }

    /* Set options, if passed */
    if(args.isSet("verbose")) importer->addFlags(Trade::ImporterFlag::Verbose);
    Implementation::setOptions(*importer, "AnySceneImporter", args.value("importer-options"));

    /* Wow, C++, you suck. This implicitly initializes to random shit?! */
    std::chrono::high_resolution_clock::duration importTime{};

    /* Open the file or map it if requested */
    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT))
    Containers::Optional<Containers::Array<const char, Utility::Path::MapDeleter>> mapped;
    if(args.isSet("map")) {
        Trade::Implementation::Duration d{importTime};
        if(!(mapped = Utility::Path::mapRead(args.value("input"))) || !importer->openMemory(*mapped)) {
            Error() << "Cannot memory-map file" << args.value("input");
            return 3;
        }
    } else
    #endif
    {
        Trade::Implementation::Duration d{importTime};
        if(!importer->openFile(args.value("input"))) {
            Error() << "Cannot open file" << args.value("input");
            return 3;
        }
    }

    /* Print file info, if requested */
    if(isInfoRequested(args)) {
        const bool error = SceneTools::Implementation::printInfo(useColor, useColor24, args, *importer, importTime);

        if(args.isSet("profile")) {
            Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds";
        }

        return error ? 1 : 0;
    }

    if(!importer->meshCount()) {
        Error{} << "No meshes found in" << args.value("input");
        return 1;
    }

    Containers::Optional<Trade::MeshData> mesh;

    /* Concatenate input meshes, if requested */
    if(args.isSet("concatenate-meshes")) {
        Containers::Array<Containers::Optional<Trade::MeshData>> meshes{importer->meshCount()};
        for(std::size_t i = 0; i != meshes.size(); ++i) if(!(meshes[i] = importer->mesh(i))) {
            Error{} << "Cannot import mesh" << i;
            return 1;
        }

        /* If there's a scene, use it to flatten mesh hierarchy. If not, assume
           all meshes are in the root. */
        /** @todo make it possible to choose the scene */
        if(importer->defaultScene() != -1) {
            Containers::Optional<Trade::SceneData> scene;
            if(!(scene = importer->scene(importer->defaultScene()))) {
                Error{} << "Cannot import scene" << importer->defaultScene() << "for mesh concatenation";
                return 1;
            }

            /** @todo once there are 2D scenes, check the scene is 3D */
            Containers::Array<Containers::Optional<Trade::MeshData>> flattenedMeshes;
            for(const Containers::Triple<UnsignedInt, Int, Matrix4>& meshTransformation: SceneTools::flattenMeshHierarchy3D(*scene))
                arrayAppend(flattenedMeshes, MeshTools::transform3D(*meshes[meshTransformation.first()], meshTransformation.third()));
            meshes = std::move(flattenedMeshes);
        }

        /* Concatenate all meshes together */
        /** @todo some better way than having to create a whole new array of
            references with the nasty NoInit, yet keeping the flexibility? */
        Containers::Array<Containers::Reference<const Trade::MeshData>> meshReferences{NoInit, meshes.size()};
        for(std::size_t i = 0; i != meshes.size(); ++i)
            meshReferences[i] = *meshes[i];
        /** @todo this will assert if the meshes have incompatible primitives
            (such as some triangles, some lines), or if they have
            loops/strips/fans -- handle that explicitly */
        mesh = MeshTools::concatenate(meshReferences);

    /* Otherwise import just one */
    } else {
        Trade::Implementation::Duration d{importTime};
        if(!(mesh = importer->mesh(args.value<UnsignedInt>("mesh"), args.value<UnsignedInt>("level")))) {
            Error{} << "Cannot import the mesh";
            return 4;
        }
    }

    /* Wow, C++, you suck. This implicitly initializes to random shit?! */
    std::chrono::high_resolution_clock::duration conversionTime{};

    /* Filter attributes, if requested */
    if(!args.value("only-attributes").empty()) {
        const Containers::Optional<Containers::Array<UnsignedInt>> only = Utility::String::parseNumberSequence(args.value<Containers::StringView>("only-attributes"), 0, mesh->attributeCount());
        if(!only) return 2;

        /** @todo use MeshTools::filterOnlyAttributes() once it has a rvalue
            overload that transfers ownership */
        Containers::Array<Trade::MeshAttributeData> attributes;
        arrayReserve(attributes, only->size());
        for(UnsignedInt i: *only)
            arrayAppend(attributes, mesh->attributeData(i));

        const Trade::MeshIndexData indices{mesh->indices()};
        const UnsignedInt vertexCount = mesh->vertexCount();
        mesh = Trade::MeshData{mesh->primitive(),
            mesh->releaseIndexData(), indices,
            mesh->releaseVertexData(), std::move(attributes),
            vertexCount};
    }

    /* Remove duplicates, if requested */
    if(args.isSet("remove-duplicates")) {
        const UnsignedInt beforeVertexCount = mesh->vertexCount();
        {
            Trade::Implementation::Duration d{conversionTime};
            mesh = MeshTools::removeDuplicates(*std::move(mesh));
        }
        if(args.isSet("verbose"))
            Debug{} << "Duplicate removal:" << beforeVertexCount << "->" << mesh->vertexCount() << "vertices";
    }

    /* Remove duplicates with fuzzy comparison, if requested */
    /** @todo accept two values for float and double fuzzy comparison */
    if(!args.value("remove-duplicates-fuzzy").empty()) {
        const UnsignedInt beforeVertexCount = mesh->vertexCount();
        {
            Trade::Implementation::Duration d{conversionTime};
            mesh = MeshTools::removeDuplicatesFuzzy(*std::move(mesh), args.value<Float>("remove-duplicates-fuzzy"));
        }
        if(args.isSet("verbose"))
            Debug{} << "Fuzzy duplicate removal:" << beforeVertexCount << "->" << mesh->vertexCount() << "vertices";
    }

    /* Assume there's always one passed --converter option less, and the last
       is implicitly AnySceneConverter. All converters except the last one are
       expected to support ConvertMesh and the mesh is "piped" from one to the
       other. If the last converter supports ConvertMeshToFile instead of
       ConvertMesh, it's used instead of the last implicit AnySceneConverter. */
    for(std::size_t i = 0, converterCount = args.arrayValueCount("converter"); i <= converterCount; ++i) {
        /* Load converter plugin */
        const Containers::StringView converterName = i == converterCount ?
            "AnySceneConverter"_s : args.arrayValue<Containers::StringView>("converter", i);
        Containers::Pointer<Trade::AbstractSceneConverter> converter = converterManager.loadAndInstantiate(converterName);
        if(!converter) {
            Debug{} << "Available converter plugins:" << ", "_s.join(converterManager.aliasList());
            return 2;
        }

        /* Set options, if passed */
        if(args.isSet("verbose")) converter->addFlags(Trade::SceneConverterFlag::Verbose);
        if(i < args.arrayValueCount("converter-options"))
            Implementation::setOptions(*converter, "AnySceneConverter", args.arrayValue("converter-options", i));

        /* This is the last --converter (or the implicit AnySceneConverter at
           the end), output to a file and exit the loop */
        if(i + 1 >= converterCount && (converter->features() & Trade::SceneConverterFeature::ConvertMeshToFile)) {
            /* No verbose output for just one converter */
            if(converterCount > 1 && args.isSet("verbose"))
                Debug{} << "Saving output (" << Debug::nospace << (i+1) << Debug::nospace << "/" << Debug::nospace << converterCount << Debug::nospace << ") with" << converterName << Debug::nospace << "...";

            Trade::Implementation::Duration d{conversionTime};
            if(!converter->convertToFile(*mesh, args.value("output"))) {
                Error{} << "Cannot save file" << args.value("output");
                return 5;
            }

            break;

        /* This is not the last converter, expect that it's capable of
           ConvertMesh */
        } else {
            CORRADE_INTERNAL_ASSERT(i < converterCount);
            if(converterCount > 1 && args.isSet("verbose"))
                Debug{} << "Processing (" << Debug::nospace << (i+1) << Debug::nospace << "/" << Debug::nospace << converterCount << Debug::nospace << ") with" << converterName << Debug::nospace << "...";

            if(!(converter->features() & Trade::SceneConverterFeature::ConvertMesh)) {
                Error{} << converterName << "doesn't support mesh conversion, only" << converter->features();
                return 6;
            }

            Trade::Implementation::Duration d{conversionTime};
            if(!(mesh = converter->convert(*mesh))) {
                Error{} << converterName << "cannot convert the mesh";
                return 7;
            }
        }
    }

    if(args.isSet("profile")) {
        Debug{} << "Import took" << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(importTime).count())/1.0e3f << "seconds, conversion"
            << UnsignedInt(std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime).count())/1.0e3f << "seconds";
    }
}
