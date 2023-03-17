# FGC REWRITER
## Getting Started Document

# FGC REWRITER build steps

Supports:
- x86_32/x86_64 on Linux.
- x86_32/x86_64 on Windows.

## Linux Build Steps
- Use the following commands for building on the target machine

```
$ cd <path to clone>
$ mkdir build
$ cd build
$ cmake ..
$ make
```

- Executable fgc_rewriter_app is generated in build directory.
- Edit the config_fgcr.cfg and run the following command.

```
$ ./fgc_rewriter_app --config config_fgcr.cfg
```

## Windows Build Steps

### Build using command line

- Create a buildtree

  ```bash
    fgcRewriterPath=<Path to fgc-rewriter repo>
    buildTree=<Name of buildTree folder>
    cd ${fgcRewriterPath}
    cmake . -B ${buildTree} -A x64
  ```

- Build using buildtree

  ```bash
    cmake --build ${buildTree} -- -p:Configuration="Release"
  ```

- Install

  ```bash
    cmake --install ${buildTree}
  ```

- Command to run the executable

  ```bash
    ./fgc_rewriter_app.exe --config config_fgcr.cfg
  ```

### Build using Visual Studio

- Use the following commands for building on the target machine

```
$ cd <path to clone>
$ mkdir build
$ cd build
$ cmake ..
```
- Open .sln (solution) in Visual Studio
```bash
  Use F7 or build using Release/Debug options
```

- Command to run the executable

```bash
  ./fgc_rewriter_app.exe --config config_fgcr.cfg
```

## Parameters

| Parameter                                  | Description                                                                                                                                                                                                                                                                      |
|--------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| --input                                    | Input H.264 file                                                                                                                                                                                                                                                                       |
| --output                                   | Output H.264 file                                                                                                                                                                                                                                                                      |
| --enable_logs                              | Enable log prints. This is used to print profile information. Supported values are 0 and 1. By default it is disabled.                                                                                                                                                           |
| --num_frames                               | Number of frames to be processed                                                                                                                                                                                                                                                 |
| --fgs_enable_rewriter                      | Flag to enable FGS rewriter. Supported values are 0 and 1. By default it is enabled.                                                                                                                                                                                             |
| --fgs_overide_fgc_cancel_flag              | Flag to override film_grain_characteristics_cancel_flag. Supported values are 0 and 1. By default it is enabled. If FGS is present in the stream and this is disabled, the value present in the stream is taken.                                                                 |
| --fgs_overide_log2_scale_factor            | Flag to override log2_scale_factor. Supported values are 0 and 1. By default it is enabled. If FGS is present in the stream and this is disabled, the value present in the stream is taken.                                                                                      |
| --film_grain_characteristics_cancel_flag   | Flag to cancel the FGS. Supported values are 0 and 1. By default it is 0.                                                                                                                                                                                                        |
| --separate_colour_description_present_flag | Flag to set separate colour description flag. Currently only 0 is supported for this value.                                                                                                                                                                                      |
| --log2_scale_factor                        | Scale factor used in FGS. Supported range is 2-7. By default it is 2                                                                                                                                                                                                             |
| --fgs_overide_dep_comp_model_values        | Flag to override all the parameters on which comp_model_value is dependent i.e. all the below parameters in this table. Supported values are 0 and 1. By default it is enabled. If FGS is present in the stream and this is disabled, the values present in the stream is taken. |
| --comp_model_present_flag                  | Control for component model for each component                                                                                                                                                                                                                                   |
| --num_intensity_intervals_minus1           | Number of intensity intervals in each component                                                                                                                                                                                                                                  |
| --num_model_values_minus1                  | 0-2 : Number of model values in each component                                                                                                                                                                                                                                   |
| --intensity_interval_lower_bound           | Lower bound of intensity interval : 0-255                                                                                                                                                                                                                                        |
| --intensity_interval_upper_bound           | Upper bound of intensity interval: 0-255                                                                                                                                                                                                                                         |
| --comp_model_value                         | Component model values for each intensity interval                                                                       

## **License**
A BSD 3-Clause-Clear license is applicable. Refer to the [LICENSE](https://github.com/ittiam-systems/libfgc-rewriter/blob/main/LICENSE) file together with the [LICENSE2](https://github.com/ittiam-systems/libfgc-rewriter/blob/main/LICENSE2) file.
