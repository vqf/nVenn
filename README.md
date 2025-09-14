# nVenn2
The nVenn tool creates generalized, proportional Venn and Euler diagrams. This second version represents a major refactoring of the algorithm to make it faster and easier to use, and to make the results simpler. 

This new version contains several functions that can be used as an API to generate custom interfaces. First, the `simulate()` function creates a diagram using the complete algorithm synchronously. Second, several functions allow programmers to perform the same actions as `simulate()` in an asynchronously. Briefly, setStep() will leave the object ready for a step, setCycle() will perform each cycle of the step and isStepFinished() will inform whether the conditions for ending the step have been met. If an error occurred, err() will be set to true, and errorMsg() will return a string. When used with a graphical interface, refresh() is true a given number of cycles after the last true value. It can be used to avoid drawing each step.
 Thus, if bl is an exported borderLine object,

    <pseudocode>
        for step in 1:7{
            setStep(step)
            quit = false
            while !quit{
                setCycle(step)
                if refresh(){
                    draw_svg_result
                }
                if isStepFinished(step){
                    quit = true
                }
            }
        }
    </pseudocode>

The API also contains functions to change the appearance of the SVG result (rotation, colors, opacity, ...). Like the first version, nVenn2 is coded in `C++` with `CodeBlocks`. The graphical interface has been recoded using `glut` for easier and more portable compiling.
The `nVennR` folder contains an interface for `R`, and the `vqf/nVennPy` repository contains an interface for `Python`.
