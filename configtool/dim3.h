/**
 * @author Alexander Entinger, BSc
 * @brief this class represents a 3 dimensional point or vector
 * @file dim3.h
 */

#ifndef DIM3_H_
#define DIM3_H_

#include <iostream>

class dim3 {
public:
	/**
	 * @brief Constructor
	 */
	dim3(float const x, float const y, float const z) : m_x(x), m_y(y), m_z(z) { }
	/**
	 * @brief Default Constructor
	 */
	dim3() : m_x(0.0f), m_y(0.0f), m_z(0.0f) { }
	/**
	 * @brief Copy-Ctor
	 */
	dim3(dim3 const &other);
	/**
	 * @brief Destructor
	 */
	~dim3() { }

	/**
	 * @brief accessor methods
	 */
	inline float x() const { return m_x; }
	inline float y() const { return m_y; }
	inline float z() const { return m_z; }

	/**
	 * @brief assignment operator
	 */
	dim3 operator = (dim3 const &other);
	/**
	 * @brief overloading + operator
	 */
	dim3 operator + (dim3 const &right) const;
	/**
	 * @brief overloading - operator
	 */
	dim3 operator - (dim3 const &right) const;
	/**
	 * @brief calculates the cross product of two dim3's
	 */
	static dim3 cross(dim3 const &left, dim3 const &right);

	/**
	 * @brief writes the configuration in a output stream for displaying it to the user
	 */
	friend std::ostream &operator<<(std::ostream& os, dim3 const &d);

private:
	float m_x, m_y, m_z;
};

#endif
