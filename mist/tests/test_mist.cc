#include <gtest/gtest.h>
#include <physics/Physics.hpp>

TEST(MistTest, collisionDetectionTest) {
	mist::Physics physics;

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(3, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(3, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 5, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,-1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(1, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 1) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(3, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(3, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 3, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::SphereCollider(1) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 3, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::SphereCollider(1) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_TRUE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 3, 0));
		mist::Transform transformB(glm::vec3(0, 0, 0));
		mist::Collider colliderA { mist::BoxCollider(glm::vec3(1,1,1)) };
		mist::Collider colliderB { mist::PlaneCollider(glm::vec3(0,1,0), 0) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}

	{
		mist::Transform transformA(glm::vec3(0, 0, 0));
		mist::Transform transformB(glm::vec3(0, 3, 0));
		mist::Collider colliderA { mist::PlaneCollider(glm::vec3(0,1,0), 0) };
		mist::Collider colliderB { mist::BoxCollider(glm::vec3(1,1,1)) };

		mist::IntersectData data = physics.DetectCollision(transformA, colliderA, transformB, colliderB);
		EXPECT_FALSE(data.isIntersecting);
	}
}