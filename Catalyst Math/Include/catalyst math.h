#pragma once

#include <math.h>

#define PI 3.1415926535897932384626433832795
#define ToRadians(angle) ((angle * PI) / 180.0f)
#define ToDegrees(angle) ((angle * 180.0f) / PI)

namespace Catalyst
{
	namespace Math
	{
		class vec2f 
		{
		public:
			union 
			{
				struct 
				{
					float X, Y;
				};

				struct 
				{
					float U, V;
				};

				float Elements[2];
			};

		public:
			vec2f(float x = 0, float y = 0)
				: X(x), Y(y)
			{}
			~vec2f() = default;
		public:
			bool operator==(vec2f &other) 
			{
				if (X == other.X && Y == other.Y)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			bool operator!=(vec2f & other)
			{
				if (X != other.X || Y != other.Y)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			void operator+=(vec2f &other)
			{
				X += other.X;
				Y += other.Y;
			}

			void operator-=(vec2f &other)
			{
				X -= other.X;
				Y -= other.Y;
			}
			
			void operator*=(vec2f &other)
			{
				X *= other.X;
				Y *= other.Y;
			}
			
			void operator/=(vec2f &other)
			{
				X /= other.X;
				Y /= other.Y;
			}

			void operator*=(float &other)
			{
				X *= other;
				Y *= other;
			}

			void operator/=(float &other)
			{
				X /= other;
				Y /= other;
			}

			float Length()
			{
				return sqrtf((X * X) + (Y * Y));
			}

			float LengthSquared()
			{
				return (X * X) + (Y * Y);
			}
		};

		class vec3f
		{
		public:
			union
			{
				struct
				{
					float X, Y, Z;
				};

				struct
				{
					float R, G, B;
				};

				float Elements[3];
			};

		public:
			vec3f(float x = 0, float y = 0, float z = 0)
				: X(x), Y(y), Z(z)
			{}

			vec3f(float x = 0, vec2f &value = vec2f())
				: X(x), Y(value.X), Z(value.Y)
			{}

			vec3f(vec2f &value = vec2f(), float z = 0)
				: X(value.X), Y(value.Y), Z(z)
			{}

			~vec3f() = default;
		public:
			bool operator==(vec3f &other)
			{
				if (X == other.X && Y == other.Y && Z == other.Z)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			bool operator!=(vec3f & other)
			{
				if (X != other.X || Y != other.Y || Z != other.Z)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			void operator+=(vec3f &other)
			{
				X += other.X;
				Y += other.Y;
				Z += other.Z;
			}

			void operator-=(vec3f &other)
			{
				X -= other.X;
				Y -= other.Y;
				Z -= other.Z;
			}

			void operator*=(vec3f &other)
			{
				X *= other.X;
				Y *= other.Y;
				Z *= other.Z;
			}

			void operator/=(vec3f &other)
			{
				X /= other.X;
				Y /= other.Y;
				Z /= other.Z;
			}

			void operator*=(float &other)
			{
				X *= other;
				Y *= other;
				Z *= other;
			}

			void operator/=(float &other)
			{
				X /= other;
				Y /= other;
				Z /= other;
			}

			float Length() 
			{
				return sqrtf((X * X) + (Y * Y) + (Z * Z));
			}

			float LengthSquared() 
			{
				return (X * X) + (Y * Y) + (Z * Z);
			}
		};

		class vec4f
		{
		public:
			union
			{
				struct
				{
					float X, Y, Z;
				};

				struct
				{
					float R, G, B;
				};

				float Elements[3];
			};

		public:
			vec4f(float x = 0, float y = 0, float z = 0)
				: X(x), Y(y), Z(z)
			{}

			vec4f(float x = 0, vec2f &value = vec2f())
				: X(x), Y(value.X), Z(value.Y)
			{}

			vec4f(vec2f &value = vec2f(), float z = 0)
				: X(value.X), Y(value.Y), Z(z)
			{}

			~vec4f() = default;
		public:
			bool operator==(vec4f &other)
			{
				if (X == other.X && Y == other.Y && Z == other.Z)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			bool operator!=(vec4f & other)
			{
				if (X != other.X || Y != other.Y || Z != other.Z)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			void operator+=(vec4f &other)
			{
				X += other.X;
				Y += other.Y;
				Z += other.Z;
			}

			void operator-=(vec4f &other)
			{
				X -= other.X;
				Y -= other.Y;
				Z -= other.Z;
			}

			void operator*=(vec4f &other)
			{
				X *= other.X;
				Y *= other.Y;
				Z *= other.Z;
			}

			void operator/=(vec4f &other)
			{
				X /= other.X;
				Y /= other.Y;
				Z /= other.Z;
			}

			void operator*=(float &other)
			{
				X *= other;
				Y *= other;
				Z *= other;
			}

			void operator/=(float &other)
			{
				X /= other;
				Y /= other;
				Z /= other;
			}

			float Length()
			{
				return sqrtf((X * X) + (Y * Y) + (Z * Z));
			}

			float LengthSquared()
			{
				return (X * X) + (Y * Y) + (Z * Z);
			}
		};

		class mat2f
		{
		
		};

		class mat3f
		{
		
		};

		class mat4f
		{
		
		};

		class Ray
		{
		
		};

		class Plane
		{
		
		};

		namespace Vec2
		{
		
		}
		
		namespace Vec3
		{
		
		}

		namespace Vec4 
		{
		
		}

		namespace Mat2
		{
		
		}

		namespace Mat3
		{
		
		}

		namespace Mat4 
		{
		
		}

		namespace Ray
		{
		
		}

		namespace Plane
		{
		
		}
	}
}