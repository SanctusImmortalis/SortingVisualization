#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>

#define NUM 100

sem_t s1, s2;
pthread_t t;
struct timespec tim;
int active = 1;

void* sort(void* arg);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const char* vertexShade = "#version 330 core\nlayout (location = 0) in vec3 aPos;\nvoid main()\n{\n gl_Position = vec4(aPos, 1.0);\n}\0";
const char* fragShade = "#version 330 core\nout vec4 FragColor;\nvoid main()\n{\n FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n}\0";


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void initArray(int* arr){
  int aux, r;
  srand(time(NULL));
  for(int i = 0;i<NUM;i++){
    arr[i] = i+1;
  }
  for(int i=0;i<NUM;i++){
    r = rand() % NUM;
    aux = arr[i];
    arr[i] = arr[r];
    arr[r] = aux;
  }
}

int main(){
  int array[NUM];
  float vertices[NUM*6];
  initArray(array);

  tim.tv_sec = 0;
  tim.tv_nsec = 50000000;

  for(int i = 0;i<(NUM*6);i+=6){
    vertices[i] = ((i/6)*((2.0f)/NUM))-1;
    vertices[i+1] = -1;
    vertices[i+2] = 0;
    vertices[i+3] = vertices[i];
    vertices[i+4] = (array[i/6]*((2.0f)/NUM)) - 1;
    vertices[i+5] = 0;
  }

  sem_init(&s1, 0, 0);
  sem_init(&s2, 0, 0);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL test", NULL, NULL);
  if(window == NULL){
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
    glfwTerminate();
    return -2;
  }
  //glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);


  int check = 0;

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShade, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &check);

  if(!check) return -3;

  check = 0;

  unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragShade, NULL);
  glCompileShader(fragShader);

  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &check);

  if(!check) return -4;

  unsigned int program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &check);

  if(!check) return -5;



  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);







  unsigned int VBO, VAO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  pthread_create(&t, NULL, sort, (void*) array);

  while(!glfwWindowShouldClose(window)){
    //processInput(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glBindVertexArray(VAO);

    glDrawArrays(GL_LINES, 0, NUM*2);

    glfwSwapBuffers(window);
    glfwPollEvents();

    if(active){

      sem_post(&s1);
      sem_wait(&s2);

      for(int i = 0;i<(NUM*6);i+=6){
        vertices[i+4] = (array[i/6]*((2.0f)/NUM)) - 1;
      }

      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
  glViewport(0, 0, width, height);
}

void* sort(void* arg){
  int* arr = (int*) arg;

  for(int i=(NUM-1);i>=0;i--){
    int j = i;
    while(true){
      sem_wait(&s1);
      int temp = 0;
      int aux = 2*j + 1;
      if(aux>=NUM) {
        sem_post(&s2);
        nanosleep(&tim, NULL);
        break;
      }
      if((aux+1)>=NUM){
        if(arr[aux]>arr[j]){
          temp = arr[aux];
          arr[aux] = arr[j];
          arr[j] = temp;
          j = aux;
        }else{
          sem_post(&s2);
          nanosleep(&tim, NULL);
          break;
        }
      }else{
        if(arr[aux+1]>arr[aux]) aux++;
        if(arr[aux]>arr[j]){
          temp = arr[aux];
          arr[aux] = arr[j];
          arr[j] = temp;
          j = aux;
        }else{
          sem_post(&s2);
          nanosleep(&tim, NULL);
          break;
        }
      }
      sem_post(&s2);
      nanosleep(&tim, NULL);
    }
  }

  for(int i=(NUM-1);i>0;i--){
    sem_wait(&s1);
    int tmp = arr[i];
    arr[i] = arr[0];
    arr[0] = tmp;
    sem_post(&s2);
    nanosleep(&tim, NULL);
    int j = 0;
    while(true){
      sem_wait(&s1);
      int temp = 0;
      int aux = 2*j + 1;
      if(aux>=i) {
        sem_post(&s2);
        nanosleep(&tim, NULL);
        break;
      }
      if((aux+1)>=i){
        if(arr[aux]>arr[j]){
          temp = arr[aux];
          arr[aux] = arr[j];
          arr[j] = temp;
          j = aux;
        }else{
          sem_post(&s2);
          nanosleep(&tim, NULL);
          break;
        }
      }else{
        if(arr[aux+1]>arr[aux]) aux++;
        if(arr[aux]>arr[j]){
          temp = arr[aux];
          arr[aux] = arr[j];
          arr[j] = temp;
          j = aux;
        }else{
          sem_post(&s2);
          nanosleep(&tim, NULL);
          break;
        }
      }
      sem_post(&s2);
      nanosleep(&tim, NULL);
    }
  }

  active = 0;
  sem_post(&s2);

}
