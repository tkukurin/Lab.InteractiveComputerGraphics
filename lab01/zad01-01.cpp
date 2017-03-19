#include <iostream>
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;

glm::vec3 example1() {
    glm::vec3 v1_first(2.0, 3.0, -4.0);
    glm::vec3 v1_second(-1.0, 4.0, -1.0);
    return v1_first + v1_second;
}

double example2(glm::vec3 v1) {
    return glm::dot(v1, glm::vec3(-1.0, 4.0, -1.0));
}

glm::vec3 example3(glm::vec3 v1) {
    glm::vec3 multiplier(2.0, 2.0, 4.0);
    return glm::cross(v1, multiplier);
}

glm::vec3 example4(glm::vec3 v2) {
    // NOTE: glupo su napravili da je v2.length() == dimenzija
    // dok je glm::length norma
    return v2 / (float) glm::length(v2);
}

glm::vec3 example5(glm::vec3 v2) {
    return -v2;
}

glm::mat3 first_matrix() {
    return glm::mat3(1, 2, 4, 2, 1, 5, 3, 3, 1);
}

glm::mat3 second_matrix() {
    return glm::mat3(-1, 5, -4, 2, -2, -1, -3, 7, 3);
}

glm::mat3 example6() {
    return first_matrix() + second_matrix();
}

glm::mat3 example7() {
    return first_matrix() * glm::transpose(second_matrix());
}

glm::mat3 example8() {
    return first_matrix() * glm::inverse(second_matrix());
}

int main(void) {
    glm::vec3 v1 = example1();
    double s = example2(v1);
    glm::vec3 v2 = example3(v1);
    glm::vec3 v3 = example4(v2);
    glm::vec3 v4 = example5(v2);
    glm::mat3 m1 = example6();
    glm::mat3 m2 = example7();
    glm::mat3 m3 = example8();
    
    cout << glm::to_string(v1) << endl;
    cout << s << endl;
    cout << glm::to_string(v2) << endl;
    cout << glm::to_string(v3) << endl;
    cout << glm::to_string(v4) << endl;
    cout << glm::to_string(m1) << endl;
    cout << glm::to_string(m2) << endl;
    cout << glm::to_string(m3) << endl;

    return 0;
}
