package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strconv"
)

// Luis Wilson A00226649
var manyDirectories, manySymbolic, manyDevices, manySockets, manyOthers int

func getMaxColumnLength(dir string) int {
	max := len(dir)
	if len(strconv.Itoa(manyDirectories)) > max {
		max = len(strconv.Itoa(manyDirectories))
	}
	if len(strconv.Itoa(manySymbolic)) > max {
		max = len(strconv.Itoa(manySymbolic))
	}
	if len(strconv.Itoa(manyDevices)) > max {
		max = len(strconv.Itoa(manyDevices))
	}
	if len(strconv.Itoa(manySockets)) > max {
		max = len(strconv.Itoa(manySockets))
	}
	if len(strconv.Itoa(manyOthers)) > max {
		max = len(strconv.Itoa(manyOthers))
	}
	return max
}

func getDottedLines(max int) string {
	output := ""

	for i := 0; i < max; i++ {
		output += "-"
	}

	output += "-+"
	return output
}

func getValueFormat(max int, value string) string {
	output := ""

	for i := 0; i < (max - len(value)); i++ {
		output += " "
	}

	output += " |"

	return output
}

// scanDir stands for the directory scanning implementation
func scanDir(dir string) error {

	err := filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			fmt.Printf("Error %v at path %q\n", err, path)
			os.Exit(1)
		}

		switch mode := info.Mode(); {

		case mode.IsDir():
			manyDirectories++
		case mode&os.ModeSymlink != 0:
			manySymbolic++
		case mode&os.ModeSocket != 0:
			manySockets++
		case mode&os.ModeDevice != 0:
			manyDevices++
		default:
			manyOthers++
		}

		return err
	})

	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	max := getMaxColumnLength(dir)

	fmt.Println("Directory Scanner Tool")
	fmt.Printf("+-------------------------+-%s\n", getDottedLines(max))
	fmt.Printf("| Path                    | %s%s\n", dir, getValueFormat(max, dir))
	fmt.Printf("+-------------------------+-%s\n", getDottedLines(max))
	fmt.Printf("| Directories             | %d%s\n", manyDirectories, getValueFormat(max, strconv.Itoa(manyDirectories)))
	fmt.Printf("| Symbolic Links          | %d%s\n", manySymbolic, getValueFormat(max, strconv.Itoa(manySymbolic)))
	fmt.Printf("| Devices                 | %d%s\n", manyDevices, getValueFormat(max, strconv.Itoa(manyDevices)))
	fmt.Printf("| Sockets                 | %d%s\n", manySockets, getValueFormat(max, strconv.Itoa(manySockets)))
	fmt.Printf("| Other files             | %d%s\n", manyOthers, getValueFormat(max, strconv.Itoa(manyOthers)))
	fmt.Printf("+-------------------------+-%s\n", getDottedLines(max))

	return err
}

func main() {

	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}

	scanDir(os.Args[1])

}
