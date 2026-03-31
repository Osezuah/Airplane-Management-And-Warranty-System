# Use the official Postgres image
FROM postgres:17-alpine

# Set the timezone to Ontario for accurate timestamps
ENV TZ=America/Toronto

# Copy schema and seed scripts
COPY ./db/01_schema.sql /docker-entrypoint-initdb.d/
COPY ./db/02_seed_data.sql /docker-entrypoint-initdb.d/

# Expose the standard Postgres port
EXPOSE 5432