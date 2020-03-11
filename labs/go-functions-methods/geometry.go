// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

//Luis Wilson

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

// Point
type Point struct{ x, y float64 }

func (p Point) X() float64 {
	return p.x
}

func (p Point) Y() float64 {
	return p.y
}

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			toAdd := path[i-1].Distance(path[i])
			sum += toAdd
			fmt.Print(" + ", toAdd)
		} else if i == 0 {
			toAdd := path[i].Distance(path[len(path)-1])
			sum += toAdd
			fmt.Print(toAdd)
		}
	}
	return sum
}

//Based on https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
func onSegment(p, q, r Point) bool {
	if (q.X() <= math.Max(p.X(), r.X())) && (q.X() >= math.Min(p.X(), r.X())) && (q.Y() <= math.Max(p.Y(), r.Y())) && (q.Y() >= math.Min(p.Y(), r.Y())) {
		return true
	}
	return false
}

//Based on https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
func orientation(p, q, r Point) int {
	val := (q.Y()-p.Y())*(r.X()-q.X()) - (q.X()-p.X())*(r.Y()-q.Y())
	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	}
	return 0
}

//Based on https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
func doIntersect(p1, q1, p2, q2 Point) bool {
	o1 := orientation(p1, q1, p2)
	o2 := orientation(p1, q1, q2)
	o3 := orientation(p2, q2, p1)
	o4 := orientation(p2, q2, q1)

	if o1 != o2 && o3 != o4 {
		return true
	}

	if o1 == 0 && onSegment(p1, p2, q1) {
		return true
	}

	if o2 == 0 && onSegment(p1, q2, q1) {
		return true
	}

	if o3 == 0 && onSegment(p2, p1, q2) {
		return true
	}

	if o4 == 0 && onSegment(p2, q1, q2) {
		return true
	}

	return false
}

func intersects(currentVertices Path, candidate Point, lastPoint int, isLast bool) bool {
	if lastPoint == -1 || lastPoint == 0 || lastPoint == 1 {
		//fmt.Println("Default acceptance!")
		return false
	}

	if isLast {
		for i := 1; i < lastPoint-1; i++ { //<= ??
			/*fmt.Print("Testing final:")
			fmt.Print(i)
			fmt.Print(", ")
			fmt.Println(i + 1)
			/*fmt.Println(currentVertices[i])
			fmt.Println(currentVertices[i+1])
			fmt.Println(currentVertices[lastPoint])
			fmt.Println(currentVertices[0])*/
			if doIntersect(currentVertices[i], currentVertices[i+1], currentVertices[lastPoint], currentVertices[0]) {
				//fmt.Println("FAILED")
				return true
			}
		}

		return false

	}

	for i := 0; i < lastPoint-1; i++ {
		/*fmt.Println("Testing:")
		fmt.Print(i)
		fmt.Print(", ")
		fmt.Println(i + 1)
		/*fmt.Println(currentVertices[i])
		fmt.Println(currentVertices[i+1])
		fmt.Println(currentVertices[lastPoint])
		fmt.Println(candidate)*/
		if doIntersect(currentVertices[i], currentVertices[i+1], currentVertices[lastPoint], candidate) {
			//fmt.Println("FAILED")
			return true
		}
	}

	return false

}

func crtVertices(sides int) Path {
	restart := true
	currentVertices := make(Path, sides)
	times := 0

	for restart {
		restart = false
		lastPoint := -1

		for i := 0; i <= sides; i++ {

			if i < sides {
				var vertex Point
				intersection := true
				tries := 0

				for intersection && tries < 100 {
					vertex.x = -100 + rand.Float64()*200
					vertex.y = -100 + rand.Float64()*200
					intersection = intersects(currentVertices, vertex, lastPoint, false)
					//fmt.Println(intersection)
					//fmt.Println(vertex)
					tries++
				}

				if tries < 100 {
					/*fmt.Print("Accepted point: ")
					fmt.Println(vertex)
					fmt.Print("with many tries: ")
					fmt.Println(tries)*/
					tries = 0
					lastPoint++
					currentVertices[lastPoint] = vertex
				} else {
					restart = true
					//fmt.Println("restarting...")
					times++
					currentVertices = make([]Point, sides)
					break
				}

			} else if i == sides {
				//fmt.Println("Connecting with last point...")
				//fmt.Println(len(currentVertices))
				var dummy Point
				dummy.x = 200
				dummy.y = 200
				if intersects(currentVertices, dummy, sides-1, true) {
					//fmt.Println("It intersected...")
					restart = true
					//fmt.Println("restarting...")
					times++
					currentVertices = make([]Point, sides)
				}
			}

		}

	}

	return currentVertices
}

func main() {
	sides, _ := strconv.Atoi(os.Args[1])

	if sides < 3 {
		fmt.Println("Cannot create figure with that many points.")
	} else {
		rand.Seed(time.Now().UnixNano())

		vertices := crtVertices(sides)
		fmt.Println("Generating a [", sides, "] sides figure")
		fmt.Println("Figure's vertices")
		for _, v := range vertices {
			fmt.Print("( ", v.X(), ", ")
			fmt.Println(v.Y(), ")")
		}

		fmt.Println("Figure's perimeter")
		perimeter := vertices.Distance()
		fmt.Println(" =", perimeter)

	}

}

//!-path
