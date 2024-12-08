import json
import sys

OUTCOMES_JSON = sys.argv[1]
OUTPUT_HTML = sys.argv[2]
PREFIX_PATH = sys.argv[3]

# Read the JSON data from outcome.json
with open(OUTCOMES_JSON, "r") as file:
    data = json.load(file)

# Generate the HTML content
html_content = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <title>Leaderboard</title>
    <link rel="stylesheet" href="bootstrap.min.css">
</head>
<body>
    <div class="container mt-5">
        <h1 class="mb-4">King Of The Grid Leaderboard</h1>
        <table class="table table-bordered table-striped">
            <thead>
                <tr>
                    <th>Bot Name</th>
                    <th>Score</th>
                    <th>Wins</th>
                </tr>
            </thead>
            <tbody>
"""

# Generate table rows from the JSON data
for index, entry in enumerate(data):
    bot_name = entry["bot"]
    score = entry["score"]
    wins = entry["wins"]
    win_links = "<br/>".join([f'<a href="results.html#{PREFIX_PATH}{win}" target="_blank">{win}</a>' for win in wins])

    html_content += f"""
                <tr>
                    <td>{index+1}. {bot_name}</td>
                    <td>{score}</td>
                    <td>{win_links if wins else "No Wins"}</td>
                </tr>
    """

# Close the HTML tags
html_content += """
            </tbody>
        </table>
    </div>
</body>
</html>
"""

# Save the HTML content to a file
with open(OUTPUT_HTML, "w") as file:
    file.write(html_content)

print("HTML page generated successfully as 'outcome.html'.")
