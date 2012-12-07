/**
 * @author Alexander Entinger, BSc
 * @brief this class represents a 3 dimensional point or vector
 * @file dim3.cpp
 */

#include "dim3.h"

/**
 * @brief Copy-Ctor
 */
dim3::dim3(dim3 const &other) {
	this->m_x = other.x();
	this->m_y = other.y();
	this->m_z = other.z();
}

/**
 * @brief assignment operator
 */
dim3 dim3::operator =(dim3 const &other) {
	if (this != &other) { // avoid self assignment
		this->m_x = other.x();
		this->m_y = other.y();
		this->m_z = other.z();
	}
	return *this;
}
/**
 * @brief overloading + operator
 */
dim3 dim3::operator +(dim3 const &right) const {
	return dim3(this->x() + right.x(), this->y() + right.y(),
			this->z() + right.z());
}
/**
 * @brief overloading - operator
 */
dim3 dim3::operator -(dim3 const &right) const {
	return dim3(this->x() - right.x(), this->y() - right.y(),
			this->z() - right.z());
}

/**
 * @brief calculates the cross product of two dim3's
 */
dim3 dim3::cross(dim3 const &left, dim3 const &right) {
	float x_res = 0.0f, y_res = 0.0f, z_res = 0.0f;

	x_res = left.y() * right.z() - left.z() * right.y();
	y_res = left.z() * right.x() - left.x() * right.z();
	z_res = left.x() * right.y() - left.y() * right.x();

	return dim3(x_res, y_res, z_res);
}

/**
 * @brief writes the configuration in a output stream for displaying it to the user
 */
std::ostream &operator<<(std::ostream& os, dim3 const &d) {
	os << "(" << d.x() << ", " << d.y() << ", " << d.z() << ")";
	return os;
}
